// ENFi32 Modern UI Webpack Configuration
const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const CopyWebpackPlugin = require('copy-webpack-plugin');

module.exports = (env, argv) => {
    const isDev = argv.mode === 'development';
    
    return {
        entry: './src/app.js',
        output: {
            path: path.resolve(__dirname, '../static/modern'),
            filename: isDev ? 'app.dev.js' : 'app.min.js',
            publicPath: '/modern/',
            clean: true
        },
        
        resolve: {
            alias: {
                'react': 'preact/compat',
                'react-dom': 'preact/compat'
            },
            extensions: ['.js', '.jsx']
        },
        
        module: {
            rules: [
                {
                    test: /\.jsx?$/,
                    exclude: /node_modules/,
                    use: {
                        loader: 'babel-loader'
                    }
                },
                {
                    test: /\.css$/i,
                    use: ['style-loader', 'css-loader']
                },
                {
                    test: /\.(png|svg|jpg|jpeg|gif)$/i,
                    type: 'asset/resource'
                }
            ]
        },
        
        plugins: [
            new HtmlWebpackPlugin({
                template: './src/index.html',
                filename: isDev ? 'index.dev.html' : 'index.html',
                inject: 'body',
                minify: !isDev
            }),
            
            new CopyWebpackPlugin({
                patterns: [
                    { 
                        from: 'src/assets', 
                        to: 'assets',
                        noErrorOnMissing: true
                    }
                ]
            })
        ],
        
        devServer: {
            static: {
                directory: path.join(__dirname, '../static')
            },
            compress: true,
            port: 3000,
            hot: true,
            proxy: {
                '/json': 'http://192.168.1.100', // ESP32 IP
                '/control': 'http://192.168.1.100',
                '/config': 'http://192.168.1.100'
            }
        },
        
        optimization: {
            splitChunks: {
                chunks: 'all',
                cacheGroups: {
                    vendor: {
                        test: /[\\/]node_modules[\\/]/,
                        name: 'vendors',
                        chunks: 'all'
                    }
                }
            }
        },
        
        devtool: isDev ? 'eval-source-map' : 'source-map'
    };
};