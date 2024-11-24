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

const delay = (ms: number): Promise<void> =>
    new Promise((resolve) => setTimeout(resolve, ms));

const rotate = async ({
    motorIndex,
    degrees,
}: {
    motorIndex?: number;
    degrees: number;
}): Promise<void> => {
    try {
        if (motorIndex === undefined) {
            throw new Error('motorIndex is required');
        }
        console.log(`Rotating motor: ${motorIndex} to ${degrees}`);
        const response = await axios.post('http://192.168.139.215/rotate/', {
            motorIndex: motorIndex?.toString(),
            degrees,
        });
        console.log('Response:', response.data);
    } catch (error: unknown) {
        if (error instanceof Error) {
            console.error('Error1:', error.message);
        } else {
            console.error('Error2:', error);
        }
    }
    await delay(200);
};
const move = async ({ type }: { type: string }): Promise<void> => {
    try {
        console.log(`Moving: ${type}`);
        const response = await axios.post('http://192.168.139.215/move/', {
            type,
        });
        console.log('Response:', response.data);
    } catch (error: unknown) {
        if (error instanceof Error) {
            console.error('Error1:', error.message);
        } else {
            console.error('Error2:', error);
        }
    }
};

const standUp = async (): Promise<void> => {
    for (let i = 0; i < 6; i++) {
        await rotate({ motorIndex: i, degrees: 30 });
    }
};

const moveLegForward = async ({
    topMotorIndex,
}: {
    topMotorIndex: number;
}): Promise<void> => {
    await rotate({ motorIndex: topMotorIndex - 6, degrees: 30 });
    await rotate({ motorIndex: topMotorIndex, degrees: 70 });
};

const moveLegBackwards = async ({
    topMotorIndex,
}: {
    topMotorIndex: number;
}): Promise<void> => {
    await rotate({ motorIndex: topMotorIndex - 6, degrees: 60 });
    await rotate({ motorIndex: topMotorIndex, degrees: 110 });
};

const walk = async (): Promise<void> => {
    await rotate({ motorIndex: 0, degrees: 60 });
    await rotate({ motorIndex: 6, degrees: 110 });
    await rotate({ motorIndex: 2, degrees: 60 });
    await rotate({ motorIndex: 8, degrees: 110 });
    await rotate({ motorIndex: 4, degrees: 60 });
    await rotate({ motorIndex: 10, degrees: 110 });

    await rotate({ motorIndex: 1, degrees: 30 });
    await rotate({ motorIndex: 7, degrees: 70 });
    await rotate({ motorIndex: 3, degrees: 30 });
    await rotate({ motorIndex: 9, degrees: 70 });
    await rotate({ motorIndex: 5, degrees: 30 });
    await rotate({ motorIndex: 11, degrees: 70 });

    await rotate({ motorIndex: 0, degrees: 30 });
    await rotate({ motorIndex: 6, degrees: 70 });
    await rotate({ motorIndex: 2, degrees: 30 });
    await rotate({ motorIndex: 8, degrees: 70 });
    await rotate({ motorIndex: 4, degrees: 30 });
    await rotate({ motorIndex: 10, degrees: 70 });

    await rotate({ motorIndex: 1, degrees: 60 });
    await rotate({ motorIndex: 7, degrees: 110 });
    await rotate({ motorIndex: 3, degrees: 60 });
    await rotate({ motorIndex: 9, degrees: 110 });
    await rotate({ motorIndex: 5, degrees: 60 });
    await rotate({ motorIndex: 11, degrees: 110 });
};

const execute = async (): Promise<void> => {
    // await move({ type: 'reset' });
    await standUp();
    // await moveLegForward({ topMotorIndex: 6 });
    // await moveLegForward({ topMotorIndex: 8 });
    // await moveLegForward({ topMotorIndex: 10 });
    // await moveLegBackwards({ topMotorIndex: 7 });
    // await moveLegBackwards({ topMotorIndex: 9 });
    // await moveLegBackwards({ topMotorIndex: 11 });

    // await moveLegForward({ topMotorIndex: 7 });
    // await moveLegBackwards({ topMotorIndex: 10 });
    // await moveLegBackwards({ topMotorIndex: 6 });
    // await moveLegForward({ topMotorIndex: 11 });
    // await moveLegBackwards({ topMotorIndex: 8 });
    // await moveLegForward({ topMotorIndex: 10 });
};
void execute();
