# mbinit
Project-Generator for mariebuild based projects

## Introduction
**Usage**
`mbinit [-t TEMPLATE_NAME] -n PROJECT_NAME` 

The default template is `c`.

### Configuration
mbinit is configured using the `mbinit_config.h` file in the root of the
repository by defining symbols. What symbol is used for what is documented
within the file.

### Templates
mbinit looks for a build-file template and init-script in the configured
template home. The extensions for the build-file template and init-script
as well as the template home are configured in `mbinit_config.h`. To find
these files mbinit simply appends the specified extensions to the given
template name.
