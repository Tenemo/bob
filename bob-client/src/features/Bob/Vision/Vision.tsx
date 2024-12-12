import { Box, Button, Typography, CircularProgress } from '@mui/material';
import { OpenAI } from 'openai';
import { zodResponseFormat } from 'openai/helpers/zod';
import React, { useState, useCallback } from 'react';
import { z } from 'zod';

import { getPrompt } from 'features/Bob/getPrompt';
import { useLazyCaptureQuery } from 'features/BobApi/bobApi';

type VisionProps = {
    isRealtimeConnected: boolean;
    children?: (
        getDescription: () => Promise<string>,
        isLoading: boolean,
    ) => React.ReactNode;
};

const ImageObjectSchema = z.object({
    objectName: z.string(),
    relativePosition: z.string(),
    distance: z.string(),
    description: z.string(),
});

const ImageAnalysisSchema = z.object({
    objects: z.array(ImageObjectSchema),
});

type ImageObject = z.infer<typeof ImageObjectSchema>;

const VISION_PROMPT: string = getPrompt('capture');

const Vision = ({
    isRealtimeConnected,
    children,
}: VisionProps): React.JSX.Element => {
    const [description, setDescription] = useState<ImageObject[]>([]);
    const [error, setError] = useState<string>('');
    const [isLoading, setIsLoading] = useState<boolean>(false);

    const [triggerCapture, { isFetching }] = useLazyCaptureQuery();

    const convertBlobToBase64 = useCallback(
        async (blob: Blob): Promise<string> =>
            new Promise<string>((resolve, reject) => {
                const reader = new FileReader();
                reader.onloadend = (): void => {
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

        const completion = await client.beta.chat.completions.parse({
            model: 'gpt-4o-2024-11-20',
            messages: [
                {
                    role: 'user',
                    content: [
                        {
                            type: 'text',
                            text:
                                VISION_PROMPT +
                                "\nPlease wrap the array in an 'objects' property of a JSON object.",
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
            response_format: zodResponseFormat(
                ImageAnalysisSchema,
                'image_analysis',
            ),
        });

        const analysisText = JSON.stringify(
            completion.choices[0].message.parsed?.objects,
        );
        try {
            const parsedObjects = JSON.parse(analysisText) as ImageObject[];
            setDescription(parsedObjects);
        } catch (err) {
            throw new Error('Failed to parse vision response', err);
        }
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
                    onClick={(): void => void handleSharePicture()}
                    sx={{ minWidth: 200 }}
                    variant="contained"
                >
                    {isFetching ? (
                        <CircularProgress size={24} />
                    ) : (
                        'Share picture'
                    )}
                </Button>

                {process.env.IS_DEBUG === 'true' && description.length > 0 && (
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
                            <pre>{JSON.stringify(description, null, 2)}</pre>
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
