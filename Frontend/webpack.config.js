const path = require('path')

module.exports = {
    mode: 'development',
    entry: './measure.js',
    output:{
        path: path.resolve(__dirname,'Frontend'),
        filename: 'bundle.js'
    },
    watch:true
}