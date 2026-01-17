" Brain F* Color highlight for vim "

if exists("b:current_syntax")
  finish
endif

syntax match BfKeywords /[<>.,]/
syntax match BfOperators  /[+\-]/
syntax match BfTerm  /[\[\]]\+/
syntax match BfPrimitives /[!#@?]/
syntax match BfComments /;.*/

highlight link BfKeywords Keyword
highlight link BfOperators Operator
highlight link BfTerm Delimiter
highlight link BfPrimitives Type
highlight link BfComments Comment
highlight link BfNumbers Number
