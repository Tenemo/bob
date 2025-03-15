import { Button } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useCallback } from 'react';

import { getPrompt } from 'features/Bob/getPrompt';

const VISION_PROMPT: string = getPrompt('vision');

type SharePictureButtonProps = {
    isConnected: boolean;
    client: RealtimeClient | null;
    getPhotoDescription: () => Promise<string>;
    setError: (error: string) => void;
};

const SharePictureButton = ({
    isConnected,
    client,
    getPhotoDescription,
    setError,
}: SharePictureButtonProps): React.JSX.Element | null => {
    const handleSharePictureClick = useCallback(async (): Promise<void> => {
        try {
            const description = await getPhotoDescription();
            client?.sendUserMessageContent([
                {
                    type: 'input_text',
                    text: `${VISION_PROMPT}\n${description}`,
                },
            ]);
        } catch (err) {
            setError(
                err instanceof Error ? err.message : 'Failed to share picture',
            );
        }
    }, [client, getPhotoDescription, setError]);

    if (!isConnected) return null;

    return (
        <Button
            onClick={(): void => {
                void handleSharePictureClick();
            }}
            variant="outlined"
        >
            Share picture
        </Button>
    );
};

export default SharePictureButton;
