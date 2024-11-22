import {
    Container,
    TextField,
    Button,
    Typography,
    Box,
    CircularProgress,
} from '@mui/material';
import React, { useState, useEffect } from 'react';
import { Helmet } from 'react-helmet-async';
import { useDispatch, useSelector } from 'react-redux';

import {
    setIp,
    setConnectionStatus,
    selectBobConnectionStatus,
} from 'features/Bob/bobSlice';
import {
    useHealthcheckQuery,
    useCaptureQuery,
} from 'features/BobApi/bobApiSlice';

const HomePage = (): React.JSX.Element => {
    const dispatch = useDispatch();
    const isConnected = useSelector(selectBobConnectionStatus);
    const [inputIp, setInputIp] = useState('');
    const [capturedImage, setCapturedImage] = useState<string | null>(null);

    const {
        data: healthcheckData,
        refetch: refetchHealthcheck,
        isFetching: isHealthcheckLoading,
    } = useHealthcheckQuery(undefined);

    const { refetch: refetchCapture, isFetching: isCaptureLoading } =
        useCaptureQuery(undefined, {
            skip: !isConnected,
        });

    useEffect(() => {
        if (healthcheckData?.status === 'OK') {
            dispatch(setConnectionStatus(true));
        } else {
            dispatch(setConnectionStatus(false));
        }
    }, [healthcheckData, dispatch]);

    const handleConnect = (): void => {
        dispatch(setIp(inputIp));
        dispatch(setConnectionStatus(false));
        refetchHealthcheck().catch((error: unknown) => {
            dispatch(setConnectionStatus(false));
            console.error('Health check failed:', error);
        });
    };

    const handleCapture = (): void => {
        refetchCapture()
            .unwrap()
            .then((response: Blob) => {
                const imageUrl = URL.createObjectURL(response);
                setCapturedImage(imageUrl);
            })
            .catch((error: unknown) => {
                console.error('Capture failed:', error);
                setCapturedImage(null);
            });
    };

    return (
        <Container maxWidth="sm">
            <Helmet>
                <title>Bob</title>
            </Helmet>
            <Box
                sx={{ mt: 4, display: 'flex', flexDirection: 'column', gap: 2 }}
            >
                <TextField
                    fullWidth
                    label="Bob IP Address"
                    onChange={(e) => {
                        setInputIp(e.target.value);
                    }}
                    value={inputIp}
                    variant="outlined"
                />
                <Button
                    color="primary"
                    disabled={!inputIp || isHealthcheckLoading}
                    onClick={handleConnect}
                    variant="contained"
                >
                    {isHealthcheckLoading ? (
                        <CircularProgress size={24} />
                    ) : (
                        'Connect'
                    )}
                </Button>
                {isConnected && healthcheckData && (
                    <Typography color="green" variant="body1">
                        Bob connected. Status: {healthcheckData.status}.
                    </Typography>
                )}
                {isConnected && (
                    <Button
                        color="secondary"
                        disabled={isCaptureLoading}
                        onClick={handleCapture}
                        variant="contained"
                    >
                        {isCaptureLoading ? (
                            <CircularProgress size={24} />
                        ) : (
                            'Capture Photo'
                        )}
                    </Button>
                )}
                {capturedImage && (
                    <Box sx={{ mt: 2, textAlign: 'center' }}>
                        <img
                            alt="Captured by Bob"
                            src={capturedImage}
                            style={{ maxWidth: '100%' }}
                        />
                    </Box>
                )}
            </Box>
        </Container>
    );
};

export default HomePage;
