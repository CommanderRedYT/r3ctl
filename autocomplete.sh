#!/bin/bash

function _r3ctl_autocomplete()
{
    local cur="${COMP_WORDS[COMP_CWORD]}"

    if [[ "$COMP_CWORD" -eq 1 ]]
    then
        COMPREPLY=( $(compgen -W "log script sendYmhButton" "$cur") )
    elif [[ "$COMP_CWORD" -eq 2 ]]
	then
		if [[ "${COMP_WORDS[1]}" = "sendYmhButton" ]]
		then
            COMPREPLY=( $(compgen -W "ymhpower ymhcd ymhtuner ymhtape ymhwdtv ymhsattv ymhvcr ymh7 ymhaux ymhextdec ymhtest ymhtunabcde ymheffect ymhtvlolup ymhtvvoldown ymhvoldown ymhvolup ymhmute ymhsleep ymhp5 ymhtunminus ymhtunplus ymhprgup ymhprgdown ymhminus ymhplus ymhtimelevel ymhmenu" "$cur") )
		fi
    fi
}
complete -F _r3ctl_autocomplete r3ctl