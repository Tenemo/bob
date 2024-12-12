import {
    TextField,
    Button,
    Typography,
    Box,
    CircularProgress,
} from '@mui/material';
import React from 'react';

import { useAppSelector, useAppDispatch } from 'app/hooks';
import { setIp, selectBobIp } from 'features/Bob/bobSlice';
import { useHealthcheckQuery, useCaptureQuery } from 'features/BobApi/bobApi';

const BobActions = (): React.JSX.Element => {
    const dispatch = useAppDispatch();
    const {
        data: healthcheckData,
        refetch: refetchHealthcheck,
        isFetching: isHealthcheckLoading,
    } = useHealthcheckQuery(undefined);

    const isBobUp = healthcheckData?.status === 'OK';
    const bobIp = useAppSelector(selectBobIp);

    const {
        refetch: refetchCapture,
        isFetching: isCaptureLoading,
        data: captureData,
    } = useCaptureQuery(undefined, {
        skip: !isBobUp,
    });

    const handleConnect = (): void => {
        void refetchHealthcheck();
    };

    return (
        <>
            <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
                <TextField
                    fullWidth
                    label="Bob's IP Address"
                    onChange={(e) => {
                        dispatch(setIp(e.target.value));
                    }}
                    value={bobIp}
                    variant="outlined"
                />
                <Button
                    color="primary"
                    disabled={!bobIp || isHealthcheckLoading}
                    onClick={handleConnect}
                    variant="contained"
                >
                    {isHealthcheckLoading ? (
                        <CircularProgress size={24} />
                    ) : isBobUp ? (
                        'Double-check if Bob is alive'
                    ) : (
                        'Check if Bob is alive'
                    )}
                </Button>

                {isBobUp && !isHealthcheckLoading && (
                    <Typography color="green" variant="body1">
                        Bob connected. Status: {healthcheckData.status}
                    </Typography>
                )}
            </Box>

            {isBobUp &&
                !isHealthcheckLoading &&
                process.env.IS_DEBUG === 'true' && (
                    <Box sx={{ mt: 4 }}>
                        <Button
                            color="secondary"
                            disabled={isCaptureLoading}
                            onClick={() => void refetchCapture()}
                            variant="contained"
                        >
                            {isCaptureLoading ? (
                                <CircularProgress size={24} />
                            ) : (
                                'Take Photo'
                            )}
                        </Button>
                    </Box>
                )}
            {captureData && process.env.IS_DEBUG && (
                <Box sx={{ mt: 2, textAlign: 'center' }}>
                    <img
                        alt="Captured by Bob"
                        src={captureData}
                        style={{ maxWidth: '60%' }}
                    />
                </Box>
            )}
        </>
    );
};

export default BobActions;
