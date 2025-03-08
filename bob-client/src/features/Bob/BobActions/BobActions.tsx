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
import { useLazyHealthcheckQuery } from 'features/BobApi/bobApi';

const BobActions = (): React.JSX.Element => {
    const dispatch = useAppDispatch();
    const [
        triggerHealthcheck,
        { data: healthcheckData, isFetching: isHealthcheckLoading },
    ] = useLazyHealthcheckQuery();

    const isBobUp = healthcheckData?.status === 'OK';
    const bobIp = useAppSelector(selectBobIp);

    const handleConnect = (): void => {
        void triggerHealthcheck(undefined, false);
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
                <Button
                    color="primary"
                    disabled={!bobIp || isHealthcheckLoading}
                    onClick={handleConnect}
                    variant={isBobUp ? 'outlined' : 'contained'}
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
                        Bob connected. Status: {healthcheckData.status}.{' '}
                        {healthcheckData.apiKey && 'API key received.'}
                    </Typography>
                )}
            </Box>
        </>
    );
};

export default BobActions;
