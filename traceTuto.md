add 
`--profiling-format=google-trace`
`--profiling-output=cmake-configuration-stage-profiling-data.json`
to cmake command

to add the arguments to cmake on vscode, add 
```
"cmake.configureArgs": [
    "--profiling-format=google-trace",
    "--profiling-output=profilerOutput.json"
],
```
in the setting.json


it produce a json in the build dir

load it on Chrome's `about:tracing` page to see the results