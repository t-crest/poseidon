#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

java -cp ${DIR}/../lib/converter.jar converter.Converter $1 $2 $3
