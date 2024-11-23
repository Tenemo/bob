import { Express, ErrorRequestHandler } from 'express';
import winston, { Logger } from 'winston';
// import * as Sentry from '@sentry/node';
// import * as Tracing from '@sentry/tracing';
import expressWinston from 'express-winston';
import DailyRotateFile from 'winston-daily-rotate-file';
import morgan, { StreamOptions } from 'morgan';

// TODO: don't disable this rule
/* eslint-disable @typescript-eslint/restrict-template-expressions */
const winstonFormat = winston.format.combine(
    winston.format.timestamp(),
    winston.format.errors({ stack: true }),
    winston.format.splat(),
    winston.format.printf(({ timestamp, level, message, stack }) => {
        return stack
            ? `${timestamp} [${level}] ${message}\n${stack}`
            : `${timestamp} [${level}] ${message}`;
    }),
);
/* eslint-enable @typescript-eslint/restrict-template-expressions */

export const setupLogging = (
    app: Express,
): {
    errorLogger: ErrorRequestHandler;
    logger: Logger;
    // sentryErrorHandler: ErrorRequestHandler;
} => {
    // Sentry.init({
    //     dsn: 'https://fe2d28a5bda54932b1914fdb2e81ab4c@o502294.ingest.sentry.io/4504889416089600',
    //     integrations: [
    //         // enable HTTP calls tracing
    //         new Sentry.Integrations.Http({ tracing: true }),
    //         // enable Express.js middleware tracing
    //         new Tracing.Integrations.Express({ app }),
    //     ],

    //     // Set tracesSampleRate to 1.0 to capture 100%
    //     // of transactions for performance monitoring.
    //     // We recommend adjusting this value in production
    //     tracesSampleRate: 1.0,
    // });

    // // RequestHandler creates a separate execution context using domains, so that every
    // // transaction/span/breadcrumb is attached to its own Hub instance
    // app.use(Sentry.Handlers.requestHandler());
    // // TracingHandler creates a trace for every incoming request
    // app.use(Sentry.Handlers.tracingHandler());

    const winstonConfig = {
        format: winstonFormat,
        transports: [
            new winston.transports.Console({
                level: 'http',
            }),
            new DailyRotateFile({
                filename: 'logs/combined-%DATE%.log',
                datePattern: 'YYYY-MM-DD',
                zippedArchive: true,
                maxSize: '20m',
                maxFiles: '14d',
                level: 'http',
            }),
            new DailyRotateFile({
                filename: 'logs/error-%DATE%.log',
                datePattern: 'YYYY-MM-DD',
                zippedArchive: true,
                maxSize: '20m',
                maxFiles: '30d',
                level: 'error',
            }),
        ],
    };

    const logger = winston.createLogger(winstonConfig);

    const morganStream: StreamOptions = {
        write: (message: string) => {
            logger.info(message.trim());
        },
    };
    app.use(morgan('combined', { stream: morganStream }));

    const errorLogger = expressWinston.errorLogger({
        format: winstonFormat,
        transports: [
            new winston.transports.Console({
                level: 'error',
            }),
        ],
    });

    return {
        errorLogger,
        logger,
        // sentryErrorHandler: Sentry.Handlers.errorHandler(),
    };
};
