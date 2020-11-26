General Information for Developers {#doc_developers}
====================================================

[TOC]

This page contains some general information for LibrePCB developers. Please also read our
contribution guidelines: https://github.com/LibrePCB/LibrePCB/blob/master/CONTRIBUTING.md


# Unstable Version {#doc_developers_unstable_versions}

As explained in the @ref doc_release_workflow, the `master` branch is always
unstable. And unstable doesn't mean "If you have bad luck, something doesn't
work as expected". It rather means "Everything you save with an unstable
version can't be opened again with any other (stable or unstable) version!!!".
So it's highly recommended to use a dedicated workspace for testing purposes,
then the files in your productive workspace are safe. When working with real
projects or libraries, make a backup first or at least use a version control
system so any modifications can be reverted afterwards.

Unstable application versions show a warning message box on every startup to
protect users from (accidentylly) beaking their files (for example if they are
trying out a nightly build). For developers this message box is very annoying,
so you can set the environment variable `LIBREPCB_DISABLE_UNSTABLE_WARNING=1` to
disable it. If you use QtCreator, this variable can be set in the run
configuration, so the warning is only disabled when starting LibrePCB from
within QtCreator. But be careful with the disabled warning! :)


# Git {#doc_developers_git}

- The `master` branch must always compile and all tests have to pass (CI successful).
- Use separate branches to fix bugs or implement new features.
- Do not pack lot of changes in one commit. Make small, incremental commits instead.
- Write commit messages according this guideline: http://chris.beams.io/posts/git-commit/


# Pull Requests {#doc_developers_pullrequests}

To keep our Git history clear, simple and consistent, we use the following
strategy for working with branches and GitHub Pull Requests:

- If a branch is outdated (i.e. commits were added to `master` in the mean
  time), the branch should be updated by **rebasing** it. Please do not update
  branches by merging `master` into it (as the "Update Branch" button on the
  GitHub UI unfortunately does).
- When adding fixes or other new changes to a branch, the already pushed
  commits should be updated accordingly (e.g. modified or amended) and then
  force-pushed instead of adding new commits.

Generally it's fine to deviate from these rules as long as the branch gets
updated and cleaned up before it is ready to merge. But keep in mind that
LibrePCB maintainers might not do a detailed review before it is cleaned up,
since reviewing a clean Git history is easier.

If you are not familiar enough with Git to do the rebasing operations - no
problem! LibrePCB maintainers can help you in several ways:

- Explain you how to do the rebase operations, force-pushes etc.
- Take over your branch and do the rebase on our own, if you agree.
- For smaller changes, we might simply
  [squash-merge](https://github.blog/2016-04-01-squash-your-commits/)
  a Pull Request.


# Internationalization (i18n) {#doc_developers_i18n}

- All files (\*.cpp, \*.h, \*.md, \*.lp, \*.ini,...) must be written in english. Only strings which
  are visible in the GUI should be translatable into other languages.
- Always use international date/time formats in files:
    - Use the [ISO 8601] format for date/time in all files.
    - Use UTC time in all files, e.g. `2014-12-20T14:42:30Z`.
    - In the GUI, you should normally use the local date/time format.
    - See also https://en.wikipedia.org/wiki/ISO_8601 and http://doc.qt.io/qt-5/qdatetime.html
- All numbers which are stored in files (e.g. S-Expressions or INI) must have a
  locale-independent format. We always use the point '.' as decimal separator,
  and no thousands separator. Example: `123456789.987654321`


# Licenses {#doc_developers_licenses}

- All 3rd-party modules and source code (e.g. from the internet) must be compatible with the GNU GPLv3
  license. This applies to all kinds of resources (images, symbols, text, sound, source code, ...).


# Exceptions {#doc_developers_exceptions}

- Use always our own exception types (librepcb::Exception and derivated classes), see [exceptions.h].
- Never use other exception types (like `std::exception`).
- Exceptions from 3rd-party libraries must be translated into our own exceptions in a wrapper class.


# Implement File Format Changes {#doc_developers_fileformat}

As documented in @ref doc_release_workflow, LibrePCB only provides a file
format upgrade path from stable major versions to the next stable major
version. Intermediate (unstable) file formats cannot be upgraded and usually
lead to errors when opening such files. However, for our unit tests and
functional tests, the libraries and projects within `./tests/data/` obviously
always need to use the latest file format, which is usually unstable
(otherwise we couldn't test the latest features). So we still need a way to
upgrade an intermediate file format to the next intermediate file format,
although only once.

In addition, LibrePCB should be able to load any previous stable file format
down to v0.1. So when implementing changes to the file format, we need to be
very carefully to not break any upgrade path from older file formats.

To handle these challenges, here a recommendation how to implement file format
changes:

1. Adjust the serialization according the new file format, but leave the
   deserialization at the previous file format. So the previous file format
   can be loaded, but the new file format is used when serializing objects.
   Commit these changes with an expressive commit message.
   Example: *FootprintPad: Support arbitrary pad shape*
2. Run the Python script `./tests/data/update_file_format.py`. This will
   open and save all libraries and projects used for our automated tests,
   so their file format gets migrated. Review the changes using `git diff`
   to be sure the migration worked as expected, then create a new branch on
   the Git submodule `./tests/data` and commit all changes.
   Typical commit message: *Update to latest file format*
3. Commit the submodule `./tests/data`.
   Typical commit message: *Update submodule "tests/data"*
4. Now implement deserializing the new file format by taking the parameter
   `fileFormat`, which is usually passed to deserialization functions and
   constructors, into account. Please don't analyze the passed `SExpression`
   node to decide how to load it (e.g. only loading an attribute if it exists)
   -- only take the passed file format into account to decide how to load it.
5. Important: Add unit tests to test deserializing both, the previous file
   format and the current (new) file format! **This is very important to ensure
   that the migration of older file formats works properly.** Without tests,
   the risk of migration issues is too high since migrations are performed
   only rarely, especially with the latest development version.
6. Commit the deserialization and unit tests using an expressive commit
   message.
   Example: *FootprintPad: Support deserializing arbitrary pad shape*

Notes:

- These steps are only a general guideline. In the end, every file format
  change is different, which might need different implementation strategies.
  For example, when adding new file format features which actually do *not*
  modify the file format after a migration (for example if new S-Expression
  nodes are only added if the new features are actually used), migrating the
  files in `./tests/data/` will not lead to any changes. In this case, all
  the steps above could be combined into a single step, ending up in a single
  commit.
- You should take a look at classes and their unit tests which already support
  a file format migration. At the time of writing this,
  @ref ::librepcb::library::FootprintPad is the only example and unfortunately
  didn't strictly follow the steps above ;-)
- If you don't have write access to the GitHub repository
  [librepcb-test-data](https://github.com/LibrePCB/librepcb-test-data), it's
  a bit cumbersome since you can't push the commit on the `./tests/data`
  submodule. Then you have two options:
    - Fork the submodule repository too and push your commit there. LibrePCB
      maintainers then need to merge it into the upstream repository before
      merging your actual change on the source repository.
    - Only push the commits on the source repository and ask LibrePCB
      maintainers to do the `./tests/data` migration on their own.


# Deployment {#doc_developers_deployment}

- Where to put our files on UNIX/Linux?: http://unix.stackexchange.com/questions/114407/deploying-my-application


[ISO 8601]: https://en.wikipedia.org/wiki/ISO_8601 "ISO 8601"
[exceptions.h]: @ref exceptions.h
