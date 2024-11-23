// import Joi from 'joi';
// import parseDbUrl from 'parse-database-url';
import dotenv from 'dotenv';

dotenv.config();

// type DatabaseConfig = {
//     database: string;
//     port: number;
//     host: string;
//     user: string;
//     password: string;
// };

// // eslint-disable-next-line @typescript-eslint/no-unsafe-call
// const databaseConfig: DatabaseConfig = parseDbUrl(
//     process.env.DATABASE_URL,
// ) as DatabaseConfig;

// const configSchema = Joi.object({
//     NODE_ENV: Joi.string()
//         .allow('development', 'production')
//         .default('production'),
//     PORT: Joi.number().default(8080),
//     driver: Joi.string(),
//     database: Joi.string().required().description('Postgres database name'),
//     port: Joi.number().default(5432),
//     host: Joi.string().default('localhost'),
//     user: Joi.string().required().description('Postgres username'),
//     password: Joi.string().allow('').description('Postgres password'),
// })
//     .unknown()
//     .required();

// Joi.assert(databaseConfig, configSchema);

export const config: {
    env: string;
    port: number;
    // postgres: DatabaseConfig;
} = {
    env: process.env.NODE_ENV ?? 'production',
    port: parseInt(process.env.PORT ?? '8080', 10),
    // postgres: {
    //     database: databaseConfig.database,
    //     port: databaseConfig.port,
    //     host: databaseConfig.host,
    //     user: databaseConfig.user,
    //     password: databaseConfig.password,
    // },
};
