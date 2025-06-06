import path from 'path';

import CopyPlugin from 'copy-webpack-plugin';
import CssMinimizerPlugin from 'css-minimizer-webpack-plugin';
import HtmlWebpackPlugin from 'html-webpack-plugin';
import MiniCssExtractPlugin, {
    loader as MiniCssExtractPluginLoader,
} from 'mini-css-extract-plugin';
import { BundleAnalyzerPlugin } from 'webpack-bundle-analyzer';
import { merge } from 'webpack-merge';
import 'dotenv/config';

import { commonConfig } from './webpack.common.babel';

const { ANALYZE } = process.env;

export default merge(commonConfig, {
    mode: `production`,
    output: {
        filename: `[name].[contenthash].min.js`,
        path: path.join(process.cwd(), `dist`),
        publicPath: '',
        assetModuleFilename: 'assets/[hash][ext][query]',
    },
    devtool: false,
    stats: 'normal',
    plugins: [
        new HtmlWebpackPlugin({
            title: 'Bob',
            template: `src/index.html`,
            inject: true,
            minify: {
                collapseWhitespace: true,
                removeComments: true,
                removeRedundantAttributes: true,
                removeScriptTypeAttributes: true,
                removeStyleLinkTypeAttributes: true,
                useShortDoctype: true,
                minifyCSS: true,
                minifyJS: true,
                minifyURLs: true,
            },
        }),
        new MiniCssExtractPlugin({
            filename: `[name].[contenthash].min.css`,
            chunkFilename: `[id].[contenthash].min.css`,
        }),
        new CopyPlugin({
            patterns: [
                {
                    from: 'src/_redirects',
                    to: '[name][ext]',
                    transform: (content): Buffer => {
                        const transformedContent = content
                            .toString()
                            .replace(
                                /\$BOB_IP/g,
                                process.env.BOB_IP ?? 'IP_NOT_FOUND',
                            );
                        return Buffer.from(transformedContent);
                    },
                },
            ],
        }),
        ...(ANALYZE ? [new BundleAnalyzerPlugin()] : []),
    ],
    optimization: {
        minimize: true,
        minimizer: [new CssMinimizerPlugin()],
        sideEffects: true,
        concatenateModules: true,
        nodeEnv: `production`,
        runtimeChunk: 'single',
        splitChunks: {
            cacheGroups: {
                vendor: {
                    test: /[\\/]node_modules[\\/]/,
                    name: 'vendor',
                    chunks: 'all',
                },
            },
        },
    },
    module: {
        rules: [
            {
                test: /\.(t|j)sx?$/,
                exclude: [/node_modules/],
                use: `babel-loader`,
            },
            {
                test: /\.(css|scss)$/,
                use: [
                    MiniCssExtractPluginLoader,
                    {
                        loader: `css-loader`,
                        options: {
                            modules: {
                                namedExport: false,
                                localIdentName: `[hash:base64]`,
                                auto: (resourcePath: string) =>
                                    !resourcePath.includes('node_modules') &&
                                    !resourcePath.includes('global.'),
                            },
                            sourceMap: false,
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
                            sourceMap: false,
                        },
                    },
                    {
                        loader: `sass-loader`,
                        options: {
                            sourceMap: false,
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
});
