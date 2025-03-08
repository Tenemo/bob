import {
    TextField,
    Button,
    Typography,
    Box,
    CircularProgress,
    ButtonGroup,
} from '@mui/material';
import React, { useState } from 'react';

import { useAppSelector, useAppDispatch } from 'app/hooks';
import { selectIsDebug, selectBobIp, setIp } from 'features/Bob/bobSlice';
import {
    useLazyHealthcheckQuery,
    useMoveCommandMutation,
    useHealthcheckQueryState,
} from 'features/BobApi/bobApi';

const BobActions = (): React.JSX.Element => {
    const dispatch = useAppDispatch();
    const [triggerHealthcheck] = useLazyHealthcheckQuery();
    const { data: healthcheckData, isFetching: isHealthcheckLoading } =
        useHealthcheckQueryState();
    const [moveCommand, { isLoading: isMoveLoading }] =
        useMoveCommandMutation();
    const [moveError, setMoveError] = useState<string>('');

    const isBobUp = healthcheckData?.status === 'OK';
    const bobIp = useAppSelector(selectBobIp);
    const isDebug = useAppSelector(selectIsDebug);

    const handleConnect = (): void => {
        void triggerHealthcheck(undefined, false);
    };

    const handleMoveCommand = (type: 'reset' | 'standUp' | 'wiggle'): void => {
        setMoveError('');
        moveCommand({ type })
            .unwrap()
            .catch((error: unknown) => {
                setMoveError(
                    error instanceof Error
                        ? error.message
                        : 'Failed to execute move command',
                );
            });
    };

    return (
        <>
            <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
                <TextField
                    fullWidth
                    label="Bob's IP Address"
                    onChange={(e): void => {
                        dispatch(setIp(e.target.value));
                    }}
                    value={bobIp}
                    variant="outlined"
                />
                {isDebug && (
                    <Button
                        color="primary"
                        disabled={!bobIp || isHealthcheckLoading}
                        onClick={handleConnect}
                        variant={isBobUp ? 'outlined' : 'contained'}
                    >
                        {isHealthcheckLoading ? (
                            <CircularProgress size={24} />
                        ) : isBobUp ? (
                            'Re-healthcheck'
                        ) : (
                            'Healthcheck'
                        )}
                    </Button>
                )}

                {isDebug && isBobUp && !isHealthcheckLoading && (
                    <Typography color="green" variant="body1">
                        Bob connected. Status: {healthcheckData.status}.{' '}
                        {healthcheckData.apiKey && 'API key received.'}
                    </Typography>
                )}
                {isBobUp && !isHealthcheckLoading && (
                    <Box>
                        <ButtonGroup
                            aria-label="Bob movement controls"
                            disabled={isMoveLoading}
                            variant="contained"
                        >
                            <Button
                                onClick={() => {
                                    handleMoveCommand('reset');
                                }}
                                sx={{ flex: 1 }}
                            >
                                Reset
                            </Button>
                            <Button
                                onClick={() => {
                                    handleMoveCommand('standUp');
                                }}
                                sx={{ flex: 1 }}
                            >
                                Stand up
                            </Button>
                            <Button
                                onClick={() => {
                                    handleMoveCommand('wiggle');
                                }}
                                sx={{ flex: 1 }}
                            >
                                Wiggle
                            </Button>
                        </ButtonGroup>

                        {moveError && (
                            <Typography color="error" variant="body2">
                                {moveError}
                            </Typography>
                        )}
                    </Box>
                )}
            </Box>
        </>
    );
};

export default BobActions;
