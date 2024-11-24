import { Container } from '@mui/material';
import { RealtimeClient } from '@openai/realtime-api-beta';
import React, { useState, useCallback } from 'react';
import { Helmet } from 'react-helmet-async';

import BobActions from './BobActions/BobActions';
import Realtime from './Realtime/Realtime';
import Vision from './Vision/Vision';

const HomePage = (): React.JSX.Element => {
    const [isRealtimeConnected, setIsRealtimeConnected] = useState(false);
    const [realtimeClient, setRealtimeClient] = useState<RealtimeClient | null>(
        null,
    );

    const handleRealtimeConnect = useCallback((client: RealtimeClient) => {
        setRealtimeClient(client);
        setIsRealtimeConnected(true);
    }, []);

    const handleRealtimeDisconnect = useCallback(() => {
        setRealtimeClient(null);
        setIsRealtimeConnected(false);
    }, []);

    const sendToRealtime = useCallback(
        (message: string) => {
            if (realtimeClient && isRealtimeConnected) {
                realtimeClient.sendUserMessageContent([
                    {
                        type: 'input_text',
                        text: `This is what you see: ${message}.
                        Don't say it's a picture for you, pretend like you are actually seeing that with your owns eyes.
                        You are to act as if you were on the breadboard with the camera. Do not just repeat the analysis,
                        instead inquire about more interesting objects you see, be curious and funny. Make short comments about what you see.
                        Keep your overall answer short.`,
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
                onAnalysis={(description: string) => {
                    if (isRealtimeConnected) {
                        sendToRealtime(description);
                    }
                }}
            />
            <Realtime
                onConnect={handleRealtimeConnect}
                onDisconnect={handleRealtimeDisconnect}
            />
        </Container>
    );
};

export default HomePage;
