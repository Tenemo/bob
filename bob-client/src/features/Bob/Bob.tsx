import { Container } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useCallback } from 'react';
import { Helmet } from 'react-helmet-async';

import BobActions from './BobActions/BobActions';
import Realtime from './Realtime/Realtime';
import Vision from './Vision/Vision';

const Bob = (): React.JSX.Element => {
    const [isRealtimeConnected, setIsRealtimeConnected] =
        useState<boolean>(false);
    const [realtimeClient, setRealtimeClient] = useState<RealtimeClient | null>(
        null,
    );
    const [lastDescription, setLastDescription] = useState<string>('');

    const handleRealtimeConnect = useCallback(
        (client: RealtimeClient): void => {
            setRealtimeClient(client);
            setIsRealtimeConnected(true);
        },
        [],
    );

    const handleRealtimeDisconnect = useCallback((): void => {
        setRealtimeClient(null);
        setIsRealtimeConnected(false);
    }, []);

    const sendToRealtime = useCallback(
        (message: string): void => {
            if (realtimeClient && isRealtimeConnected) {
                realtimeClient.sendUserMessageContent([
                    {
                        type: 'input_text',
                        text: message,
                    },
                ]);
            }
        },
        [realtimeClient, isRealtimeConnected],
    );

    return (
        <Container
            sx={{
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'flex-start',
            }}
        >
            <Helmet>
                <title>Bob</title>
            </Helmet>
            <BobActions />
            <Vision
                isRealtimeConnected={isRealtimeConnected}
                onAnalysis={(description: string): void => {
                    setLastDescription(description);
                    if (isRealtimeConnected) {
                        sendToRealtime(description);
                    }
                }}
            />
            <Realtime
                initialPhotoDescription={lastDescription}
                onConnect={handleRealtimeConnect}
                onDisconnect={handleRealtimeDisconnect}
            />
        </Container>
    );
};

export default Bob;
