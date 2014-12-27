# jantman .bashrc
# copied from antmanLaptop 2008-06-06
# modified for tuxostat
# Time-stamp: "2008-06-10 09:52:25 tuxostat"

export EDITOR=/usr/bin/emacs

test -s ~/.alias && . ~/.alias || true

export UNAME="Jason"
export GENDER="M"

export CVS_RSH="/home/tuxostat/.ssh/ssh10022"
export CVSROOT=":ext:jantman@jantman.dyndns.org:/usr/local/cvsroot"

#set the PROMPT

cur_tty=$(temp=$(tty) ; echo ${temp:5});
export PS1="\u@\h:$cur_tty:\w > "
export PS2="> "

if [ -n "$TTY" ]; then

    #display relevant host information
    echo "You are $LOGNAME on $HOSTNAME."
    echo "OS: $OSTYPE"
    echo

    source ~/bin/greeting.sh
    if [ -x /usr/bin/fortune ] ; then
	echo
	/usr/bin/fortune
	echo
    fi
fi
