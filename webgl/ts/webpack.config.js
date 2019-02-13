module.exports = {
    mode: 'development',
    devtool: 'source-map',
    entry: {
      app :'./app/main.ts'
    },
    output: {
      path: `${__dirname}/dest`,
      filename: 'bundle.js'
    },
    module: {
      rules: [
        {
          test: /\.ts$/,
          exclude: /node_modules/,
          use: {
            loader: 'ts-loader'
          }
        }
      ]
    },
    resolve: { extensions: ['.ts'] }
  };