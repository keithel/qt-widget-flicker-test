Reproducer app showing flickering windows introduced in Qt6
===========================================================

This is a simple reproducer app showing a bug in Qt 6, in which an embedded
Windows native window causes the window to be repainted with any move or resize
of the toplevel window.

Testing
=======

Configure, build and test this example using Qt 5.15, and again using Qt 6.5.2.
Observe the native window messages logged, and compare the results between Qt
5.15 and Qt 6 builds.

Results
=======
With Qt 6 build, you will see WM_NCPAINT and WM_ERASEBKGND messages that are not
seen with Qt 5.15 build.

Possible solution
=================
The fix for bug QTBUG-97774 seems to be the cause of this.
Reverting commit [Windows QPZE TO AVOID JITTERNOCOPYBITS during resize to avoid jitter](https://code48f693f2a9425948a37ec45b4301mmit/?id=000f1ee3604048f693f2a9425948a37ec45b4301) fixes the issue.

