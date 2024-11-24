import { Box, Button, Typography } from '@mui/material';
import { OpenAI } from 'openai';
import React, { useState } from 'react';

import { IS_DEBUG } from 'app/config';
import { useAppSelector } from 'app/hooks';
import { selectBobIp } from 'features/Bob/bobSlice';
import { useCaptureQuery } from 'features/BobApi/bobApi';

const VISION_PROMPT = `
Describe what you see on the image in minute detail. 
Especially note distances to objects, as this is from a perspective of a movable camera.
Another system will use your prompt to navigate and respond questions about the world.`;

type VisionProps = {
    onAnalysis: (description: string) => void;
};

const Vision = ({ onAnalysis }: VisionProps): React.JSX.Element => {
    const [description, setDescription] = useState('');
    const [error, setError] = useState('');
    const [isLoading, setIsLoading] = useState(false);
    const bobIp = useAppSelector(selectBobIp);
    const { data: captureData } = useCaptureQuery(undefined, {
        skip: !bobIp,
    });

    const convertBlobToBase64 = async (blob: Blob): Promise<string> => {
        return new Promise<string>((resolve, reject) => {
            const reader = new FileReader();
            reader.onloadend = () => {
                const base64String = reader.result as string;
                resolve(base64String.split(',')[1]);
            };
            reader.onerror = reject;
            reader.readAsDataURL(blob);
        });
    };

    const analyzeImage = async (): Promise<void> => {
        if (!captureData || !process.env.OPENAI_API_KEY) {
            setError('No image available or API key missing');
            return;
        }

        setIsLoading(true);
        setError('');

        try {
            const captureDataResponse = await fetch(captureData);
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
                max_tokens: 500,
            });

            const analysisText = response.choices[0].message.content ?? '';
            setDescription(analysisText);

            // Send to realtime if connected
            onAnalysis(analysisText);
        } catch (err) {
            setError(
                err instanceof Error ? err.message : 'Failed to analyze image',
            );
        } finally {
            setIsLoading(false);
        }
    };

    return (
        <Box sx={{ mt: 4, display: 'flex', flexDirection: 'column', gap: 2 }}>
            <Button
                disabled={isLoading || !captureData}
                onClick={() => void analyzeImage()}
                variant="contained"
            >
                {isLoading ? 'Analyzing...' : 'Analyze Latest Image'}
            </Button>

            {/* eslint-disable-next-line @typescript-eslint/no-unnecessary-condition */}
            {description && !IS_DEBUG && (
                <Typography
                    sx={{
                        mt: 2,
                        p: 2,
                        bgcolor: 'background.paper',
                        borderRadius: 1,
                    }}
                    variant="body1"
                >
                    {description}
                </Typography>
            )}

            {error && <Typography color="error">Error: {error}</Typography>}
        </Box>
    );
};

export default Vision;
