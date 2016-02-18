# converts terminal.sexy`s themes to linux-vt-setcolors format
# https://github.com/stayradiated/terminal.sexy/tree/master/dist/schemes

fs = require "fs"

readFiles = (dirname, onFileContent, onError) ->
  fs.readdir dirname, (err, filenames) ->
    return onError err if err
    filenames.forEach (filename) ->
      return if filename.indexOf(".json")<0
      fs.readFile dirname + filename, 'utf-8', (err, content) ->
        return onError err if err
        onFileContent dirname, filename, content

onFile = (dirname, filename, content)->
  name = filename.replace(".json","")
  json = JSON.parse(content)
  colors=""
  for v, i in json.color
    colors += "#{i}#{v}\n"
  fs.writeFile dirname + name, colors

readFiles "./base16/", onFile, (err)->console.log err
readFiles "./collection/", onFile, (err)->console.log err
readFiles "./xcolors.net/", onFile, (err)->console.log err
