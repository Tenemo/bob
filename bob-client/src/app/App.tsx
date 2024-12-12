import 'normalize.css';

import 'fonts/RobotoMono-Regular.woff2';
import 'fonts/RobotoMono-Regular.woff';

import React from 'react';
import { ErrorBoundary } from 'react-error-boundary';
import { Helmet } from 'react-helmet-async';
import { Route, Routes } from 'react-router-dom';

import * as styles from './app.scss';

import Header from 'components/Header/Header';
import NotFound from 'components/NotFound/NotFound';
import Bob from 'features/Bob/Bob';

const App = (): React.JSX.Element => {
    return (
        <div className={styles.app}>
            <Helmet>
                <title>Bob</title>
            </Helmet>
            <ErrorBoundary
                fallback={
                    <div>
                        The application has crashed due to a rendering error.
                    </div>
                }
                onError={(error) => {
                    console.error(error);
                }}
            >
                <Header />
                <Routes>
                    <Route element={<Bob />} path="/" />
                    <Route element={<NotFound />} path="*" />
                </Routes>
            </ErrorBoundary>
        </div>
    );
};

export default App;
