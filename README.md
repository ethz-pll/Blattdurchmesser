[![Build Status](https://travis-ci.org/ethz-pll/Blattdurchmesser.svg?branch=master)](https://travis-ci.org/ethz-pll/Blattdurchmesser)

## Setup

I would like to use features like CI (continous integration) keep a copy/mirror on e.g. github and more.

ETHZ GitLab could (and may be already does) support such features but I do not understand how this works.

Maintaining a copy/mirror (more precise: with 2 remotes overloaded origin) on GitHub also allows
to use GitLab and GitHub together and by that all the nice featurs on GitHub.
See also; https://steveperkins.com/migrating-projects-from-github-to-gitlab/

"Option 2: Overload Origin with Both Remotes (needs one single pull/push only)"; in order to clone
and set this repo up use:
```
$ git clone https://gitlab.ethz.ch/PLL/Blattdurchmesser.git
$ cd Blattdurchmesser
$ git remote set-url --add origin https://github.com/ethz-pll/Blattdurchmesser.git
```
you can check the settings with:
```
$ git remote -v
origin  https://gitlab.ethz.ch/PLL/Blattdurchmesser.git (fetch)
origin  https://gitlab.ethz.ch/PLL/Blattdurchmesser.git (push)
origin  https://github.com/ethz-pll/Blattdurchmesser.git (push)
```

### Enable Travis CI for a Repository (for Arduino)
https://learn.adafruit.com/continuous-integration-arduino-and-you/testing-your-project

Login to Travis-CI using the GitHub account and enable Travis-CI for the given repo.

Add a .travis.yml file to your project, commit, pull/push and enjoy.

This travis file supports Arduino IDE 1.8.1 only we use 1.8.2.

Consider using http://inotool.org/ for test also.