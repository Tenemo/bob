import { Container } from '@mui/material';
import React from 'react';
import { Helmet } from 'react-helmet-async';

import BobActions from './BobActions/BobActions';
import Realtime from './Realtime/Realtime';

const HomePage = (): React.JSX.Element => {
    return (
        <Container>
            <Helmet>
                <title>Bob</title>
            </Helmet>
            <BobActions />
            <Realtime />
        </Container>
    );
};

export default HomePage;
