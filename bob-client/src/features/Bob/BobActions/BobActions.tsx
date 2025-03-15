import {
    Button,
    Typography,
    Box,
    CircularProgress,
    ButtonGroup,
} from '@mui/material';
import React from 'react';

import { useAppSelector } from 'app/hooks';
import { selectIsDebug } from 'features/Bob/bobSlice';
import {
    useLazyHealthcheckQuery,
    useMoveCommandMutation,
    useHealthcheckQueryState,
    MoveCommandRequest,
} from 'features/BobApi/bobApi';

const BobActions = (): React.JSX.Element => {
    const [triggerHealthcheck] = useLazyHealthcheckQuery();
    const { data: healthcheckData, isFetching: isHealthcheckLoading } =
        useHealthcheckQueryState();
    const [moveCommand, { isLoading: isMoveLoading, error: moveError }] =
        useMoveCommandMutation({
            fixedCacheKey: 'moveCommand',
        });

    const isBobUp = healthcheckData?.status === 'OK';
    const isDebug = useAppSelector(selectIsDebug);

    const handleConnect = (): void => {
        void triggerHealthcheck(undefined, false);
    };

    const handleMoveCommand = (type: MoveCommandRequest['type']): void => {
        void moveCommand({ type });
    };

    return (
        <>
            <Box
                sx={{
                    display: 'flex',
                    flexDirection: 'column',
                    gap: 2,
                    width: '100%',
                    alignItems: 'flex-start',
                }}
            >
                {isDebug && (
                    <Button
                        color="primary"
                        disabled={isHealthcheckLoading}
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
                {((isBobUp && !isHealthcheckLoading) || isDebug) && (
                    <Box>
                        <ButtonGroup
                            aria-label="Bob movement controls"
                            disabled={isMoveLoading}
                            variant="contained"
                        >
                            <Button
                                onClick={() => {
                                    handleMoveCommand('sitDown');
                                }}
                                sx={{ flex: 1 }}
                            >
                                Sit down{' '}
                                {isMoveLoading && (
                                    <CircularProgress
                                        size={20}
                                        sx={{ ml: 1 }}
                                    />
                                )}
                            </Button>
                            <Button
                                onClick={() => {
                                    handleMoveCommand('standUp');
                                }}
                                sx={{ flex: 1 }}
                            >
                                Stand up{' '}
                                {isMoveLoading && (
                                    <CircularProgress
                                        size={20}
                                        sx={{ ml: 1 }}
                                    />
                                )}
                            </Button>
                            <Button
                                onClick={() => {
                                    handleMoveCommand('wiggle');
                                }}
                                sx={{ flex: 1 }}
                            >
                                Wiggle{' '}
                                {isMoveLoading && (
                                    <CircularProgress
                                        size={20}
                                        sx={{ ml: 1 }}
                                    />
                                )}
                            </Button>
                        </ButtonGroup>

                        {moveError && (
                            <Typography color="error" variant="body2">
                                {moveError instanceof Error
                                    ? moveError.message
                                    : 'Failed to execute move command'}
                            </Typography>
                        )}
                    </Box>
                )}
            </Box>
        </>
    );
};

export default BobActions;
