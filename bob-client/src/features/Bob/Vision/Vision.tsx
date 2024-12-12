import { Box, Button, Typography, CircularProgress } from '@mui/material';
import { OpenAI } from 'openai';
import React, { useState, useCallback } from 'react';

import { getPrompt } from 'features/Bob/getPrompt';
import { useLazyCaptureQuery } from 'features/BobApi/bobApi';

type VisionProps = {
    isRealtimeConnected: boolean;
    children?: (
        getDescription: () => Promise<string>,
        isLoading: boolean,
    ) => React.ReactNode;
};

const VISION_PROMPT: string = getPrompt('capture');

const Vision = ({
    isRealtimeConnected,
    children,
}: VisionProps): React.JSX.Element => {
    const [description, setDescription] = useState<string>('');
    const [error, setError] = useState<string>('');
    const [isLoading, setIsLoading] = useState<boolean>(false);

    const [triggerCapture, { isFetching }] = useLazyCaptureQuery();

    const convertBlobToBase64 = useCallback(
        async (blob: Blob): Promise<string> =>
            new Promise<string>((resolve, reject) => {
                const reader = new FileReader();
                reader.onloadend = () => {
                    const base64String = reader.result as string;
                    resolve(base64String.split(',')[1]);
                };
                reader.onerror = reject;
                reader.readAsDataURL(blob);
            }),
        [],
    );

    const analyzeImage = useCallback(async (): Promise<string> => {
        if (!process.env.OPENAI_API_KEY) {
            throw new Error('API key missing');
        }
        setIsLoading(true);
        setError('');
        const result = await triggerCapture();
        if (!result.data) {
            throw new Error('No image to analyze');
        }
        const captureDataResponse = await fetch(result.data);
        const blob = await captureDataResponse.blob();
        const base64Image = await convertBlobToBase64(blob);
        const client = new OpenAI({
            apiKey: process.env.OPENAI_API_KEY,
            dangerouslyAllowBrowser: true,
        });
        const response = await client.chat.completions.create({
            model: 'gpt-4o-2024-11-20',
            messages: [
                {
                    role: 'user',
                    content: [
                        {
                            type: 'text',
                            text: VISION_PROMPT,
                        },
                        {
                            type: 'image_url',
                            image_url: {
                                url: `data:image/jpeg;base64,${base64Image}`,
                                detail: 'high',
                            },
                        },
                    ],
                },
            ],
            max_tokens: 4096,
        });
        const analysisText = response.choices[0].message.content ?? '';
        setDescription(analysisText);
        setIsLoading(false);
        return analysisText;
    }, [convertBlobToBase64, triggerCapture]);

    const handleSharePicture = useCallback(async (): Promise<void> => {
        try {
            await analyzeImage();
        } catch (err) {
            setError(
                err instanceof Error ? err.message : 'Failed to analyze image',
            );
            console.error(err);
            setIsLoading(false);
        }
    }, [analyzeImage]);

    const getDescription = useCallback(async (): Promise<string> => {
        return analyzeImage();
    }, [analyzeImage]);

    return (
        <>
            <Box
                sx={{
                    mt: 4,
                    display: 'flex',
                    flexDirection: 'column',
                    gap: 2,
                    width: '100%',
                }}
            >
                <Button
                    disabled={isLoading || !isRealtimeConnected}
                    onClick={() => void handleSharePicture()}
                    sx={{ minWidth: 200 }}
                    variant="contained"
                >
                    {isFetching ? (
                        <CircularProgress size={24} />
                    ) : (
                        'Share picture'
                    )}
                </Button>
                {process.env.IS_DEBUG === 'true' && description && (
                    <Box
                        sx={{
                            width: '100%',
                            mt: 2,
                            p: 2,
                        }}
                    >
                        <Typography>Photo description:</Typography>
                        <Box
                            sx={{
                                maxHeight: '400px',
                                width: '100%',
                                overflow: 'auto',
                            }}
                        >
                            <pre>
                                {JSON.stringify(
                                    JSON.parse(description),
                                    null,
                                    2,
                                )}
                            </pre>
                        </Box>
                    </Box>
                )}
                {error && <Typography color="error">Error: {error}</Typography>}
            </Box>
            {typeof children === 'function'
                ? children(getDescription, isLoading || isFetching)
                : null}
        </>
    );
};

export default Vision;
