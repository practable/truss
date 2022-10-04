// vue.config.js
module.exports = {
    // options...
    runtimeCompiler: true,
    publicPath: process.env.NODE_ENV === 'production'
    ? '/ui/truss-noforce-1.0/'
    : '/'
  }