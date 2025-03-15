import { Button, CircularProgress } from '@mui/material';
import React, { useCallback } from 'react';

import { stopAudio } from './realtimeUtils';

type ConnectionButtonProps = {
    isConnected: boolean;
    isConnectInProgress: boolean;
    isVisionLoading: boolean;
    handleConnectClick: () => Promise<void>;
    stopAudioMutation: () => Promise<unknown>;
    useBobSpeaker: boolean;
};

const ConnectionButton = ({
    isConnected,
    isConnectInProgress,
    isVisionLoading,
    handleConnectClick,
    stopAudioMutation,
    useBobSpeaker,
}: ConnectionButtonProps): React.JSX.Element => {
    const showSpinner = isConnectInProgress || isVisionLoading;

    const handleClick = useCallback((): void => {
        stopAudio(stopAudioMutation, useBobSpeaker);
        void handleConnectClick();
    }, [handleConnectClick, stopAudioMutation, useBobSpeaker]);

    return (
        <Button disabled={showSpinner} onClick={handleClick} variant="outlined">
            {showSpinner ? (
                <CircularProgress size={24} />
            ) : isConnected ? (
                'Disconnect'
            ) : (
                'Connect to Bob'
            )}
        </Button>
    );
};

export default ConnectionButton;
