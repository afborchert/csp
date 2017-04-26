# CSP
CSP is a language by C. A. R. Hoare for a process algebra that describes
patterns of interaction. The language is specified in the book
_Communicating Sequential Processes_, published by Prentice Hall 1985,
ISBN 0-13-153289, which is fortunately available online in an updated
version: <http://www.usingcsp.com/>.

The book itself provides sections that give directions how various
operators can be implemented in LISP. This package is such an
implementation, albeit based on C++ and yacc, that supports a small
subset of CSP. A utility is provided that allows to interactively follow
possible traces or to check non-interactively for a process _P_ if a
trace is member of _traces_(_P_).

## Downloading and building

Cloning should be done recursively:

```
git clone --recursive https://github.com/afborchert/csp.git
```

To build it, you need _bison_ 3.x and a recent _g++_ with
C++14 support:

```
cd csp/csp
make depend
make
```

## Lexical symbols

### Comments
The commenting style of Ada and C is supported:
* For single-line comments `//` or `--` can be used which extend to the end of the line.
* Comments can be enclosed in `/*` and `*/`.

### Identifiers and keywords
Following identifiers and keywords are used by the grammar:

| Symbol    | Description                                      | Regular expression
| --------- | ------------------------------------------------ | --------------------
| *PROCESS* | identifier that begins with an upper case letter | `[A-Z][A-Za-z0-9_]*`
| *EVENT*   | identifier that begins with a lower case letter  | `[a-z][A-Za-z0-9_]*`
| *ALPHA*   | keyword "alpha" (must be lower case)             | alpha
| *RUN*     | keyword "RUN" (must be upper case)               | RUN
| *STOP*    | keyword "STOP" (must be upper case)              | STOP
| *SKIP*    | keyword "SKIP" (must be upper case)              | SKIP

### Operators
Following operators are supported and presented in the order of precedence,
those with the highest precedence coming first:

| Operator             | Description          | Associativity   | Section
| -------------------- | -------------------- | --------------- | -------
| `(`...`)`            | grouping             |                 |
| `->`                 | prefix               | right-to-left   | 1.1.1
| &#124;~&#124;        | non-deterministic or | left-to-right   | 3.2
| `[]`                 | general choice       | left-to-right   | 3.3
| &#124;&#124;&#124;   | interleaving         | left-to-right   | 3.6
| &#124;&#124;         | concurrency          | left-to-right   | 2.3
| `;`                  | sequence             | left-to-right   | 5.1
| `=`                  | definition           | non-associative | 1.1

The right-to-left prefix operator `->` (see section 1.1.1) is
right-associative and must be used within parentheses, possibly
in combination with the `|` operator (see section 1.1.3) which
must be used with prefix expressions only.

The sections refer to the book by C. A. R. Hoare.

### Delimiters

* Alphabets are enclosed in `{` ... `}`.
* Multiple members of an alphabet are separated by commas: `,`

## Grammar
The grammar represents a subset of CSP:

   _ProcessDefinitions_ &#8594; _ProcessDefinition_ | _ProcessDefinitions_ _ProcessDefinition_

   _ProcessDefinition_ &#8594; *PROCESS* `=` _ProcessExpression_ | *PROCESS* _Alphabet_ `=` _ProcessExpression_

   _ProcessExpression_ &#8594; _ProcessSequence_

   _ProcessSequence_ &#8594; _ParallelProcesses_ | _ProcessSequence_ `;` _ParallelProcesses_

   _ParallelProcesses_ &#8594; _InterleavingProcesses_ | _ParallelProcesses_ `||` _InterleavingProcesses_

   _InterleavingProcesses_ &#8594; _ExternalChoice_ | _InterleavingProcesses_ `|||` _ExternalChoice_


   _ExternalChoice_ &#8594; _InternalChoice_ | _ExternalChoice_ `[]` _InternalChoice_

   _InternalChoice_ &#8594; _SimpleProcessExpression_ | _InternalChoice_ = `|~|` _SimpleProcessExpression_

   _SimpleProcessExpression_ &#8594; *PROCESS* | *RUN* _Alphabet_ | *RUN* *ALPHA* *PROCESS* | *STOP* _Alphabet_ | *STOP* *ALPHA* *PROCESS* | *SKIP* _Alphabet_ | *SKIP* *ALPHA* *PROCESS* | `(` _ProcessExpression_ `)` | `(` _Choices_ `)`

   _Choices_ &#8594; _PrefixExpression_ | _Choices_ `|` _PrefixExpression_

   _PrefixExpression_ &#8594; *EVENT* `->` _ProcessExpression_ | *EVENT* `->` _PrefixExpression_

   _Alphabet_ &#8594; `{` `}` | `{` _AlphabetMembers_ `}`

   _AlphabetMembers_ &#8594; *EVENT* | _AlphabetMembers_ `,` *EVENT*

# Usage
The _trace_ command expects a filename as last argument and supports following flags:

* `-a` do not print the alphabet at the beginning
* `-e` print every accepted event
* `-p` do not print the current process before the next event is read from the input
* `-v` do not print the set of acceptable events before the next event is read from the input

Typically, _trace_ is used interactively. Hence, helpful verbose output is given,
i.e. the entire alphabet at the beginning, and before the next event is read from the input,
the current process is printed and the set of acceptable events. If one of the acceptable
events is chosen, _trace_ continues. If an unacceptable event is selected, _trace_
terminates with an error message. If the input ends or the process terminates successfully,
"OK" is printed and _trace_ terminates.

If _trace_ is to be used non-interactively, it is best to use either the `-apv` flag
combination that suppresses all the verbose output, or to use `-aepv` where all accepted
events are printed.

# Examples
Following examples are all taken from C. A. R. Hoare's book. First the corresponding
section is given, then the example number within that section.

## 1.1.2 X1
```
$ cat x1.csp
-- CSP 1.1.2, X1, p. 6
CLOCK = (tick -> CLOCK)
$ trace x1.csp
Tracing: CLOCK = (tick -> CLOCK)
Alphabet: {tick}
Acceptable: {tick}
tick
Process: CLOCK = (tick -> CLOCK)
Acceptable: {tick}
tick
Process: CLOCK = (tick -> CLOCK)
Acceptable: {tick}
OK
$
```

## 1.1.2 X2
```
$ cat x2.csp
-- CSP 1.1.2 X2, p. 6
VMS = (coin -> choc -> VMS)
$ trace x2.csp
Tracing: VMS = (coin -> choc -> VMS)
Alphabet: {choc, coin}
Acceptable: {coin}
coin
Process: (choc -> VMS)
Acceptable: {choc}
choc
Process: VMS = (coin -> choc -> VMS)
Acceptable: {coin}
OK
$
```

## 1.1.2 X3
By default, _trace_ is supposed to be used interactively. If you
are just interested in checking whether a particular trace is in traces(P)
all verbose output can be switched off:

```
$ cat x3.csp
-- CSP 1.1.2 X3 p. 7
CH5A = (in5p -> out2p -> out1p -> out2p -> CH5A)
$ echo in5p out2p out1p out2p | trace -apv x3.csp
OK
$ echo in5p out2p in5p out2p | trace -apv x3.csp
cannot accept in5p
$ echo in5p out2p in5p out2p | trace -aepv x3.csp
in5p
out2p
cannot accept in5p
$
```

## 1.1.3 X1
Please note that the special processes *STOP*, *RUN*, or *SKIP*
always require the specification of an alphabet. The alphabet
can be specified with reference to another named process using
the *alpha* keyword or by enumerating the events explicitly
in braces.

```
$ cat x1.csp
-- CSP 1.1.3 X1 p.8
P = (up -> STOP alpha P | right -> right -> up -> STOP alpha P)
$ trace x1.csp
Tracing: P = (up -> STOP {right, up} | right -> right -> up -> STOP {right, up})
Alphabet: {right, up}
Acceptable: {right, up}
right
Process: (right -> up -> STOP {right, up})
Acceptable: {right}
right
Process: (up -> STOP {right, up})
Acceptable: {up}
up
Process: STOP {right, up}
Acceptable: {}
OK
$
```

At the end we observe an empty set of acceptable events. If the input
ends subsequently, _trace_ terminates successfully. Otherwise an
error message is printed.

## 1.1.4 X1
```
$ cat x1.csp
-- CSP 1.1.4 X1 p. 11
DD = (setorange -> O | setlemon -> L)
O  = (orange -> O | setlemon -> L | setorange -> O)
L  = (lemon -> L | setorange -> O | setlemon -> L)
$ trace x1.csp
Tracing: DD = (setorange -> O | setlemon -> L)
Alphabet: {lemon, orange, setlemon, setorange}
Acceptable: {setlemon, setorange}
setlemon
Process: L = (lemon -> L | setorange -> O | setlemon -> L)
Acceptable: {lemon, setlemon, setorange}
lemon
Process: L = (lemon -> L | setorange -> O | setlemon -> L)
Acceptable: {lemon, setlemon, setorange}
lemon
Process: L = (lemon -> L | setorange -> O | setlemon -> L)
Acceptable: {lemon, setlemon, setorange}
setorange
Process: O = (orange -> O | setlemon -> L | setorange -> O)
Acceptable: {orange, setlemon, setorange}
orange
Process: O = (orange -> O | setlemon -> L | setorange -> O)
Acceptable: {orange, setlemon, setorange}
OK
$
```

## 2.2 X1
```
$ cat x1.csp
-- CSP 2.2 X1 p. 46
P = GRCUST || VMCT
GRCUST = (toffee -> GRCUST | choc -> GRCUST | coin -> choc -> GRCUST)
VMCT = (coin -> (choc -> VMCT | toffee -> VMCT)) -- see CSP 1.1.3 X3 p. 8
$ trace x1.csp
Tracing: P = GRCUST || VMCT
Alphabet: {choc, coin, toffee}
Acceptable: {coin}
coin
Process: (choc -> GRCUST) || (choc -> VMCT | toffee -> VMCT)
Acceptable: {choc}
choc
Process: GRCUST || VMCT
Acceptable: {coin}
coin
Process: (choc -> GRCUST) || (choc -> VMCT | toffee -> VMCT)
Acceptable: {choc}
choc
Process: GRCUST || VMCT
Acceptable: {coin}
OK
$
```

## 2.2 X2
The _trace_ utility determines by default the alphabet of a process from
the explicitly named events in its equations. If we let interact processes
by using the `||` operator, the alphabets of the two processes are not
extended by this operation. For this reason it was necessary in the
following example to specify the alphabet of _FOOLCUST_ explicitly:

```
$ cat x2.csp
-- CSP 2.2 X2 p. 46
P = FOOLCUST || VMC

FOOLCUST {in1p, in2p, out1p, small, large} =
   (in2p -> large -> FOOLCUST | in1p -> large -> FOOLCUST)

-- VMC comes from CSP 1.1.3 X4 p. 8 with design flaw:
--    "WARNING: do not insert three pennies in a row"
VMC = (in2p -> (large -> VMC | small -> out1p -> VMC) |
      in1p -> (small -> VMC | in1p -> (large -> VMC | in1p -> STOP alpha VMC)))
$ trace x2.csp
Tracing: P = FOOLCUST || VMC
Alphabet: {in1p, in2p, large, out1p, small}
Acceptable: {in1p, in2p}
in2p
Process: (large -> FOOLCUST) || (large -> VMC | small -> out1p -> VMC)
Acceptable: {large}
large
Process: FOOLCUST || VMC
Acceptable: {in1p, in2p}
in1p
Process: (large -> FOOLCUST) || (small -> VMC | in1p -> (large -> VMC | in1p -> STOP {in1p, in2p, large, out1p, small}))
Acceptable: {}
OK
$
```
