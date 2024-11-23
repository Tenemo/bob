import express from 'express'; // ErrorRequestHandler
// import {ErrorRequestHandler } from 'express';
import cookieParser from 'cookie-parser';
import { router } from 'routes/router';
import { config } from './config';
import cors from 'cors';
import { setupLogging } from './logging';
import axios from 'axios';

export const app = express();

app.use(cors());
app.use(express.json());
app.use(
    express.urlencoded({
        extended: true,
    }),
);
app.use(cookieParser());

const {
    errorLogger,
    logger,
    // sentryErrorHandler
} = setupLogging(app);

app.use('/api', router);

// The error handler must be BEFORE any other error middleware and AFTER all controllers
// app.use(sentryErrorHandler);

// Optional fallthrough error handler
// const onErrorSentry: ErrorRequestHandler = (_err, _req, res) => {
//     // The error id is attached to `res.sentry` to be returned
//     // and optionally displayed to the user for support.
//     res.statusCode = 500;
//     // @ts-ignore
//     res.end((res.sentry as string) + '\n');
// };

// app.use(onErrorSentry);

// express-winston errorLogger AFTER the router and AFTER sentry.
app.use(errorLogger);

console.log = (message: string) =>
    logger.log({
        level: 'info',
        message,
    });
console.warn = (message: string) =>
    logger.warn({
        level: 'warn',
        message,
    });
console.error = (message: string) =>
    logger.error({
        level: 'error',
        message,
    });

// export const server = app.listen(config.port, () => {
//     console.log(
//         `Server running on port ${config.port} in ${config.env} environment`,
//     );
// });

// Function to make a single POST request
const rotate = async ({
    motorIndex,
    degrees,
}: {
    motorIndex?: number;
    degrees: number;
}): Promise<void> => {
    try {
        const response = await axios.post('http://192.168.212.22/rotate/', {
            motorIndex,
            degrees,
        });
        console.log('Response:', response.data);
    } catch (error: unknown) {
        if (error instanceof Error) {
            console.error('Error:', error.message);
        } else {
            console.error('Error:', error);
        }
    }
};
const makeCalls = async (): Promise<void> => {
    await rotate({ degrees: 50, motorIndex: 0 });
    // await rotate({ degrees: 50, motorIndex: 15 });
};

void makeCalls();
