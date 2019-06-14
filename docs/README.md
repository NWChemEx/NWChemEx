Building the documentation
==========================

Building is done by running:

```
python3 build_docs.py
```

in the `SDE/docs` directory. If this runs successfully you will get two
additional directories: `build` and `doxyoutput`. The actual documentation can
be viewed by opening `docs/build/html/index.html` in a web browser.

If the build did not run successfully check that you have installed the
following Python packages (all available in `pip`):

- sphinx_rtd_theme (The Read-The-Docs theme for sphinx)
- sphinx (The thing that makes the documenation)
- breathe (Dependency of exhale, may get installed by it)
- exhale (The thing that turns Doxygen output into ReST)

You also will need Doxygen.
