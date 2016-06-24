#!/usr/bin/env bash

PROJECT_FOLDER=/var/www/html/profiles/wim/behat

behat --version

echo $PROJECT_FOLDER/config/behat.yml;

behat $PROJECT_FOLDER --config $PROJECT_FOLDER/config/behat.yml --tags "stability"
