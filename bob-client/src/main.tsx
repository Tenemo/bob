import { CircularProgress, CssBaseline, ThemeProvider } from '@mui/material';
import React, { useEffect } from 'react';
import { createRoot } from 'react-dom/client';
import { HelmetProvider } from 'react-helmet-async';
import { Provider } from 'react-redux';
import { BrowserRouter } from 'react-router-dom';
import { PersistGate } from 'redux-persist/integration/react';

import App from 'app/App';
import { store, persistor } from 'app/store';
import { darkTheme } from 'styles/theme';

import 'styles/global.scss';

export const Root = (): React.JSX.Element => {
    useEffect(() => {
        // https://stackoverflow.com/questions/31402576/enable-focus-only-on-keyboard-use-or-tab-press
        document.body.addEventListener('mousedown', () => {
            document.body.classList.add('using-mouse');
        });
        document.body.addEventListener('keydown', (event) => {
            if (event.key === 'Tab') {
                document.body.classList.remove('using-mouse');
            }
        });
        console.log(`Build date: ${process.env.BUILD_DATE ?? 'N/A'}`);
    }, []);

    return (
        <React.StrictMode>
            <Provider store={store}>
                <PersistGate
                    loading={<CircularProgress />}
                    persistor={persistor}
                >
                    <HelmetProvider>
                        <ThemeProvider theme={darkTheme}>
                            <CssBaseline enableColorScheme />
                            <BrowserRouter>
                                <App />
                            </BrowserRouter>
                        </ThemeProvider>
                    </HelmetProvider>
                </PersistGate>
            </Provider>
        </React.StrictMode>
    );
};

const container = document.getElementById('root');

// eslint-disable-next-line @typescript-eslint/no-non-null-assertion
const root = createRoot(container!);
root.render(<Root />);
