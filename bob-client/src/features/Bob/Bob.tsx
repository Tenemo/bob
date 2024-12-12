import { Container } from '@mui/material';
import React, { useState, useCallback } from 'react';
import { Helmet } from 'react-helmet-async';

import BobActions from './BobActions/BobActions';
import Realtime from './Realtime/Realtime';
import Vision from './Vision/Vision';

const Bob = (): React.JSX.Element => {
    const [isRealtimeConnected, setIsRealtimeConnected] =
        useState<boolean>(false);

    const handleRealtimeConnect = useCallback((): void => {
        setIsRealtimeConnected(true);
    }, []);

    const handleRealtimeDisconnect = useCallback((): void => {
        setIsRealtimeConnected(false);
    }, []);

    return (
        <Container
            sx={{
                display: 'flex',
                flexDirection: 'column',
                alignItems: 'flex-start',
                pb: 4,
            }}
        >
            <Helmet>
                <title>Bob</title>
            </Helmet>
            <BobActions />
            <Vision isRealtimeConnected={isRealtimeConnected}>
                {(
                    getPhotoDescription: () => Promise<string>,
                    isLoading: boolean,
                ): React.ReactNode => (
                    <Realtime
                        getPhotoDescription={getPhotoDescription}
                        isVisionLoading={isLoading}
                        onConnect={handleRealtimeConnect}
                        onDisconnect={handleRealtimeDisconnect}
                    />
                )}
            </Vision>
        </Container>
    );
};

export default Bob;
