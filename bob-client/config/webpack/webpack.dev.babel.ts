import ReactRefreshWebpackPlugin from '@pmmmwh/react-refresh-webpack-plugin';
import CircularDependencyPlugin from 'circular-dependency-plugin';
import HtmlWebpackPlugin from 'html-webpack-plugin';
import { address } from 'ip';
import ReactRefreshBabel from 'react-refresh/babel';
import { WatchIgnorePlugin, HotModuleReplacementPlugin } from 'webpack';
import type { Configuration } from 'webpack';
import { merge } from 'webpack-merge';
import 'dotenv/config';

import { commonConfig } from './webpack.common.babel';

const localIp = address();
const PORT = process.env.PORT ?? 3000;
const BOB_LOCAL_IP = process.env.BOB_LOCAL_IP ?? 'MISSING_IP';

const developmentConfiguration: Configuration = {
    mode: `development`,
    // devServer is still part of the configuration and has effect
    // on the server. Seems like it was mistakenly removed from types.
    // eslint-disable-next-line @typescript-eslint/ban-ts-comment
    // @ts-ignore
    devServer: {
        historyApiFallback: true,
        port: PORT as number,
        hot: true,
        headers: {
            'Access-Control-Allow-Origin': `*`,
        },
        proxy: [
            {
                context: ['/bob'],
                target: `http://${BOB_LOCAL_IP}`,
                pathRewrite: { '^/bob': '' },
                changeOrigin: true,
                secure: false,
            },
        ],
    },
    output: {
        filename: `[name].js`,
        publicPath: `http://${localIp}:${PORT.toString()}/`,
    },
    devtool: `eval-source-map`,
    plugins: [
        new HtmlWebpackPlugin({
            title: 'Bob',
            template: `src/index.html`,
            inject: true,
        }),
        new CircularDependencyPlugin({
            exclude: /a\.js|node_modules/,
            failOnError: false,
        }),
        new WatchIgnorePlugin({
            paths: [/(css|scss)\.d\.ts$/],
        }),
        new HotModuleReplacementPlugin(),
        new ReactRefreshWebpackPlugin(),
    ],
    optimization: {
        minimize: false,
        emitOnErrors: false,
    },
    module: {
        rules: [
            {
                test: /\.(t|j)sx?$/,
                exclude: [/node_modules/],
                use: [
                    {
                        loader: `babel-loader`,
                        options: { plugins: [ReactRefreshBabel] },
                    },
                ],
            },
            {
                test: /\.(css|scss)$/,
                use: [
                    `style-loader`,
                    {
                        loader: 'css-modules-typescript-loader',
                    },
                    {
                        loader: `css-loader`,
                        options: {
                            modules: {
                                namedExport: false,
                                localIdentName: `[path]_[local]`,
                                auto: (resourcePath: string) =>
                                    !resourcePath.includes('node_modules') &&
                                    !resourcePath.includes('global.'),
                            },
                            sourceMap: true,
                            importLoaders: 2,
                        },
                    },
                    {
                        loader: `postcss-loader`,
                        options: {
                            postcssOptions: {
                                plugins: [
                                    'postcss-flexbugs-fixes',
                                    'autoprefixer',
                                ],
                            },
                            sourceMap: true,
                        },
                    },
                    {
                        loader: `sass-loader`,
                        options: {
                            sourceMap: true,
                            implementation: require.resolve('sass'),
                            sassOptions: {
                                includePaths: ['src/styles'],
                            },
                        },
                    },
                ],
            },
            {
                test: /\.(woff|woff2|svg|ttf|eot)$/,
                type: 'asset/resource',
            },
            {
                test: /\.(jpe?g|png|gif|ico)$/,
                type: 'asset/resource',
            },
        ],
    },
};

export default merge(commonConfig, developmentConfiguration);
