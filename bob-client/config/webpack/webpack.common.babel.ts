import path from 'path';

import DotenvPlugin from 'dotenv-webpack';
import { EnvironmentPlugin, Configuration } from 'webpack';

import packageJSON from '../../package.json';

export const commonConfig: Configuration = {
    entry: {
        [packageJSON.name]: [
            `core-js/stable`,
            `react`,
            `react-dom`,
            path.join(process.cwd(), `src/main`),
        ],
    },
    target: `web`,
    plugins: [
        new EnvironmentPlugin({
            NODE_ENV: 'production',
            PORT: 3000,
            ANALYZE: false,
            BUILD_DATE: JSON.stringify(new Date().toISOString().split('T')[0]),
            APP_VERSION: packageJSON.version,
        }),
        new DotenvPlugin({
            systemvars: true,
            safe: './.env.sample',
            defaults: true,
        }),
    ],
    resolve: {
        extensions: [`.ts`, `.tsx`, `.js`, `.jsx`, `.css`, `.scss`],
    },
    performance: {
        hints: false,
    },
    module: {
        rules: [
            {
                test: /\.md$/,
                use: [
                    {
                        loader: 'html-loader',
                    },
                    {
                        loader: 'remark-loader',
                    },
                ],
            },
            // other rules unchanged
        ],
    },
};
