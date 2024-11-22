import {
    Container,
    TextField,
    Button,
    Typography,
    Box,
    CircularProgress,
} from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import { Mic, MicOff } from 'lucide-react';
import React, { useState, useEffect } from 'react';
import { Helmet } from 'react-helmet-async';

import { useAppDispatch, useAppSelector } from 'app/hooks';
import {
    setIp,
    setConnectionStatus,
    selectBobConnectionStatus,
    selectBobIp,
} from 'features/Bob/bobSlice';
import { useHealthcheckQuery, useCaptureQuery } from 'features/BobApi/bobApi';

const HomePage = (): React.JSX.Element => {
    const dispatch = useAppDispatch();
    const isConnected = useAppSelector(selectBobConnectionStatus);
    const storedIp = useAppSelector(selectBobIp);
    const [inputIp, setInputIp] = useState(storedIp);
    const [capturedImage, setCapturedImage] = useState<string | null>(null);
    const [client, setClient] = useState<RealtimeClient | null>(null);
    const [lastResponse, setLastResponse] = useState('');
    const [transcript, setTranscript] = useState('');
    const [isAiConnected, setIsAiConnected] = useState(false);
    const [isConnecting, setIsConnecting] = useState(false);
    const [isStreaming, setIsStreaming] = useState(false);

    const {
        data: healthcheckData,
        refetch: refetchHealthcheck,
        isFetching: isHealthcheckLoading,
    } = useHealthcheckQuery(undefined);

    const { refetch: refetchCapture, isFetching: isCaptureLoading } =
        useCaptureQuery(undefined, {
            skip: !isConnected,
        });
    const sendAudioToBob = async (audioData) => {
        if (!isConnected || !storedIp) return;

        try {
            // Convert Int16Array to ArrayBuffer for direct transmission
            const response = await fetch(`http://${storedIp}/audio`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'audio/wav',
                },
                body: audioData.buffer,
            });

            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
        } catch (error) {
            console.error('Failed to send audio to Bob:', error);
        }
    };

    const connectToOpenAI = async () => {
        setIsConnecting(true);
        try {
            const newClient = new RealtimeClient({
                apiKey: process.env.OPENAI_API_KEY,
                dangerouslyAllowAPIKeyInBrowser: true,
            });

            newClient.on('conversation.item.completed', ({ item }) => {
                console.log('Conversation item completed:', item);
                if (
                    item.type === 'message' &&
                    item.role === 'assistant' &&
                    item.formatted?.audio
                ) {
                    console.log('Sending audio to Bob...');
                    sendAudioToBob(item.formatted.audio);
                }
            });

            newClient.on('conversation.updated', ({ item, delta }) => {
                if (item.role === 'user' && delta?.transcript) {
                    setTranscript((prev) => prev + delta.transcript);
                }
                if (item.role === 'assistant' && delta?.text) {
                    setLastResponse((prev) => prev + delta.text);
                }
            });

            await newClient.connect();
            setClient(newClient);
            setIsAiConnected(true);
            console.log('Connection established successfully.');
        } catch (error) {
            console.error('Error connecting to OpenAI:', error);
        } finally {
            setIsConnecting(false);
        }
    };
    const startAudioStream = async () => {
        try {
            const stream = await navigator.mediaDevices.getUserMedia({
                audio: true,
            });
            const audioContext = new AudioContext({ sampleRate: 24000 });
            const source = audioContext.createMediaStreamSource(stream);
            const processor = audioContext.createScriptProcessor(4800, 1, 1); // 0.2 seconds at 24kHz

            let audioBuffer = new Float32Array(0);
            let silenceStart = null;
            const silenceThreshold = 0.01;
            const silenceDuration = 1000; // 1 second

            processor.onaudioprocess = (e) => {
                const inputData = e.inputBuffer.getChannelData(0);
                audioBuffer = new Float32Array([...audioBuffer, ...inputData]);

                // Check for silence
                const isCurrentlySilent = inputData.every(
                    (value) => Math.abs(value) < silenceThreshold,
                );
                if (isCurrentlySilent) {
                    if (!silenceStart) {
                        silenceStart = Date.now();
                    } else if (Date.now() - silenceStart > silenceDuration) {
                        console.log('Silence detected, creating response...');
                        client?.createResponse();
                        silenceStart = null;
                    }
                } else {
                    silenceStart = null;
                }

                // Process audio in chunks
                while (audioBuffer.length >= 4800) {
                    const chunk = audioBuffer.slice(0, 4800);
                    audioBuffer = audioBuffer.slice(4800);

                    const int16Array = new Int16Array(chunk.length);
                    for (let i = 0; i < chunk.length; i++) {
                        int16Array[i] =
                            Math.max(-1, Math.min(1, chunk[i])) * 0x7fff;
                    }

                    try {
                        client?.appendInputAudio(int16Array);
                    } catch (error) {
                        console.error('Error sending audio data:', error);
                    }
                }
            };

            source.connect(processor);
            processor.connect(audioContext.destination);

            return () => {
                processor.disconnect();
                source.disconnect();
                stream.getTracks().forEach((track) => {
                    track.stop();
                });
                audioContext.close();
            };
        } catch (error) {
            console.error('Error starting audio stream:', error);
            return null;
        }
    };
    const toggleStreaming = async (): Promise<void | null | (() => void)> => {
        if (!isStreaming) {
            const cleanup = await startAudioStream();
            setIsStreaming(true);
            return cleanup;
        } else {
            setIsStreaming(false);
            return null;
        }
    };
    useEffect(() => {
        setInputIp(storedIp);
    }, [storedIp, dispatch]);

    useEffect(() => {
        if (healthcheckData?.status === 'OK') {
            dispatch(setConnectionStatus(true));
        } else {
            dispatch(setConnectionStatus(false));
        }
    }, [healthcheckData, dispatch]);

    const disconnectFromOpenAI = (): void => {
        if (client) {
            client.disconnect();
            setClient(null);
            setIsAiConnected(false);
            setTranscript('');
            setLastResponse('');
        }
    };

    const handleConnect = (): void => {
        dispatch(setIp(inputIp));
        dispatch(setConnectionStatus(false));
        refetchHealthcheck().catch((error: unknown) => {
            dispatch(setConnectionStatus(false));
            console.error('Health check failed:', error);
        });
    };

    const handleCapture = (): void => {
        refetchCapture()
            .unwrap()
            .then((response: Blob) => {
                const imageUrl = URL.createObjectURL(response);
                setCapturedImage(imageUrl);
            })
            .catch((error: unknown) => {
                console.error('Capture failed:', error);
                setCapturedImage(null);
            });
    };

    return (
        <Container>
            <Helmet>
                <title>Bob</title>
            </Helmet>
            <Box
                sx={{ mt: 4, display: 'flex', flexDirection: 'column', gap: 2 }}
            >
                <TextField
                    fullWidth
                    label="Bob's IP Address"
                    onChange={(e) => {
                        setInputIp(e.target.value);
                    }}
                    value={inputIp}
                    variant="outlined"
                />
                <Button
                    color="primary"
                    disabled={!inputIp || isHealthcheckLoading}
                    onClick={handleConnect}
                    variant="contained"
                >
                    {isHealthcheckLoading ? (
                        <CircularProgress size={24} />
                    ) : (
                        'Connect'
                    )}
                </Button>
                {isConnected && healthcheckData && (
                    <Typography color="green" variant="body1">
                        Bob connected. Status: {healthcheckData.status}.
                    </Typography>
                )}
                {isConnected && (
                    <Button
                        color="secondary"
                        disabled={isCaptureLoading}
                        onClick={handleCapture}
                        variant="contained"
                    >
                        {isCaptureLoading ? (
                            <CircularProgress size={24} />
                        ) : (
                            'Capture Photo'
                        )}
                    </Button>
                )}
                {capturedImage && (
                    <Box sx={{ mt: 2, textAlign: 'center' }}>
                        <img
                            alt="Captured by Bob"
                            src={capturedImage}
                            style={{ maxWidth: '100%' }}
                        />
                    </Box>
                )}
            </Box>
            <Box
                sx={{
                    p: 3,
                    border: '1px solid',
                    borderColor: 'divider',
                    borderRadius: 1,
                }}
            >
                <Typography gutterBottom variant="h6">
                    OpenAI Connection
                </Typography>
                <Button
                    color="primary"
                    disabled={isConnecting}
                    onClick={
                        isAiConnected ? disconnectFromOpenAI : connectToOpenAI
                    }
                    sx={{ mr: 2 }}
                    variant="contained"
                >
                    {isConnecting
                        ? 'Connecting...'
                        : isAiConnected
                          ? 'Disconnect from OpenAI'
                          : 'Connect to OpenAI'}
                </Button>

                {isAiConnected && (
                    <Button
                        color={isStreaming ? 'error' : 'success'}
                        onClick={() => {
                            void toggleStreaming();
                        }}
                        variant="contained"
                    >
                        {isStreaming ? 'Stop Streaming' : 'Start Streaming'}
                    </Button>
                )}

                {isAiConnected && (
                    <Typography color="green" sx={{ mt: 1 }}>
                        Connected to OpenAI successfully
                    </Typography>
                )}
            </Box>

            {isAiConnected && (
                <Box
                    sx={{
                        p: 3,
                        border: '1px solid',
                        borderColor: 'divider',
                        borderRadius: 1,
                    }}
                >
                    <Typography gutterBottom variant="h6">
                        Conversation
                    </Typography>

                    {transcript && (
                        <Typography
                            sx={{ mt: 2, color: 'text.secondary' }}
                            variant="body1"
                        >
                            You: {transcript}
                        </Typography>
                    )}

                    {lastResponse && (
                        <Typography sx={{ mt: 2 }} variant="body1">
                            Assistant: {lastResponse}
                        </Typography>
                    )}
                </Box>
            )}
        </Container>
    );
};

export default HomePage;
