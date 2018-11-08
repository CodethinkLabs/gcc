C { dg-do run }
C
C Ensure that we handle the pathname on a separate line than
C the include directive using the continuation column
C
      subroutine one()
        include
     +  "include_14.inc"
        if (i4.ne.4) stop 1
      end subroutine one

      subroutine two()
      incl
     1ude
     2"in
     3clude_14.inc"
        if (i4.ne.4) stop 2
      end subroutine two

      subroutine three()
        include
C comments are actually allowed to be inserted
C between a line and its continuation!
     +  "include_14.inc"
        if (i4.ne.4) stop 1
      end subroutine three

      subroutine four()
        include
C comments are actually allowed to be inserted

C between a line and its continuation!
     +  "inc
C comment in filename


     +lude_14.inc"
        if (i4.ne.4) stop 1
      end subroutine four

      program test
        call one
        call two
        call three
        call four
      end program test
