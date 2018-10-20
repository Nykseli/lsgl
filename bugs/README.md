# LSGL Bugs

This folder contains known lsgl bugs and examples of them

### TODO bugfix:
- [x] Multiline comments
    - multiline-comment.lsgl
- [x] Changing variable value in scope works only changes the value of first parent, not all parents. (Wasn't bug, it was a feature)
    - scope-bug.lsgl
- [x] Empty string and 0 value int are seen as boolean true
    - string-int-bool-bug.lsgl
- [x] If inside of while loop results in segfault if comparison value is used in if
    - if-inside-while.lsgl
