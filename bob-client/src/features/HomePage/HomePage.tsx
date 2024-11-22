import { Container } from '@mui/material';
import React from 'react';
import { Helmet } from 'react-helmet-async';

import BobActions from './BobActions/BobActions';
import Realtime from './Realtime/Realtime';
import Vision from './Vision/Vision';

const HomePage = (): React.JSX.Element => {
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
            <Vision />
            <Realtime />
        </Container>
    );
};

export default HomePage;
