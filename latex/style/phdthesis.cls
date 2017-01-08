% $Log: phdthesis.cls,v $
% LaTeX format for theses at MIT, based on "Specifications for Thesis Preparation" 

% To use this style - say something like:
%  for dull, boring thesis format:
%	\documentclass[12pt]{phdthesis}
%       \pagestyle{plain}
% OR for fast drafts: 
%	\documentclass[11pt,singlespace,draft]{phdthesis}
%	\pagestyle{drafthead}
% OR for Tech Reports:
%	\documentclass[12pt,twoside]{phdthesis}	
%	\pagestyle{headings}
% Option:  oneside, double-spaced are the default.
% Option 'draft':  Puts `overfull' boxes at the end of lines that are too long. 
% Pagestyle 'drafthead':  Puts the date and the label ``*DRAFT*'' in the footer.
% Option 'upcase':	Will put much of the cover page in uppercase, as per the
% 		example on page 17 of the *Specifications for Thesis Preparation*, (revised 1989)
% Also added ``All Rights Reserved'' to default copyright notice.
%
%
%
%%%% Parameters to initialize for boilerplate page:
%
%	\title{Mixed Circular Cylindrical Shells}
% 	\author{J. Casey Salas}
% 	\prevdegrees{B.S., University of California (1978) \\
%		     S.M., Massachusetts Institute of Technology (1981)}
% 	\department{Department of Electrical Engineering and Computer Science}
% 	\degree{Doctor of Philosophy}
%% If the thesis is for two degrees simultaneously, list them both
%% separated by \and like this:
% 	\degree{Doctor of Philosophy \and Master of Science}
% 	\degreemonth{February}
% 	\degreeyear{1987}
% 	\thesisdate{December 10, 1986}
%% If the thesis is copyright by the Institute, leave this line out and
%% the standard copyright line will be used instead.
% 	\copyrightnotice{J. Casey Salas, 1986}
%% If there is more than one supervisor, use the \supervisor command
%% once for each.
% 	\supervisor{John D. Galli}{Director, Sound Instrument Laboratory}
%% This is the department committee chairman, not the thesis committee chairman
% 	\chairman{Arthur C. Smith}
%		 {Chairman, Departmental Committee on Graduate Students}
%% Make the titlepage based on the above information.  If you need
%% something special and can't use the standard form, you can specify
%% the exact text of the titlepage yourself.  Put it in a titlepage
%% environment and leave blank lines where you want vertical space.
%% The spaces will be adjusted to fill the entire page.  The dotted
%% lines for the signatures are made with the \signature command.
%
%% The abstractpage environment sets up everything on the page except
%% the text itself.  The title and other header material are put at the
%% top of the page, and the supervisors are listed at the bottom.  A
%% new page is begun both before and after.  Of course, an abstract may
%% be more than one page itself.  If you need more control over the
%% format of the page, you can use the abstract environment, which puts
%% the word "Abstract" at the beginning and single spaces its text.
%
% 	\begin{abstractpage}
%	    Abstract goes here.
%	\end{abstractpage}
%
%%%%%%%%%%%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{phdthesis}[1999/10/20]

\def\mystretch{1.5}		% Double spacing hack
\DeclareOption{doublespace}{}	% This is default
				% So we do not read this style twice
\DeclareOption{singlespace}{		% If he explicitly wants single spacing
    \typeout{Single spaced}
    \def\mystretch{1}}	

%% `vi' and `upcase' document style options.  Krishna Sethuraman (1990)
\newcount\vithesis
\DeclareOption{vi}{\typeout{Course VI/VIII thesis style.}\advance\vithesis by1}
\vithesis=0

\DeclareOption{upcase}{\typeout{Uppercase cover page.}
	\gdef\choosecase#1{\uppercase\expandafter{#1}}}
\def\choosecase#1{#1}

%  Thesis looks much like report
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{report}}
\ProcessOptions
\LoadClass{report}

% If the user wants single spacing, set baselinestretch=1.
% Read the doublespace style that we got from Rochester.

%\input ../style/doublespace.sty 		% We always read doublespacing. fanghui: add directory path
\def\baselinestretch{\mystretch}	% Double spacing hack



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%  Set up margins and formatting params %%%
% Margins.
%  Note we want 1in top margin assuming no header line, so push header into 1in margin.
%  Draft mode brings the header back down.

\setlength{\oddsidemargin}{0.25in}	% 1.25in left margin 
\setlength{\evensidemargin}{0.25in}	% 1.25in left margin (even pages)
\setlength{\topmargin}{0.0in}		% 1in top margin
\setlength{\textwidth}{6.0in}		% 6.0in text - 1.25in rt margin
\setlength{\textheight}{9in}		% Body ht for 1in margins
\addtolength{\topmargin}{-\headheight}	% No header, so compensate
\addtolength{\topmargin}{-\headsep}	% for header height and separation

% The next two macros compensate page style for headers and footers
% We only need them in page styles that USE headers and footers.
    % If we have a header, it must be 1in from top of page.
\def\pulldownheader{			% Shift header down 1in from top
    \addtolength{\topmargin}{\headheight}	
    \addtolength{\topmargin}{\headsep}	
    \addtolength{\textheight}{-\headheight}
    \addtolength{\textheight}{-\headsep}
}
    % If we have a footer, put it 1in up from bottom
\def\pullupfooter{				% Shift footer up
    \addtolength{\textheight}{-\footskip}
    \addtolength{\textheight}{-\footheight}
}

%%%%%%%  End of margins and formatting params %%%

%%%%%%%  Fix various header and footer problems %%%

% Format for draft of thesis.  Define our own PageStyle -
% Just like headings, but has foot lines with the date and warning

\if@twoside         % If two-sided printing.
\def\ps@drafthead{
    \let\@mkboth\markboth
    \def\@oddfoot{\rm \today \hfil \sc **DRAFT**}
    \def\@evenfoot{\sc **DRAFT** \hfil \rm \today }
    \def\@evenhead{\rm \thepage\hfil \sl \leftmark}
    \def\@oddhead{\hbox{}\sl \rightmark \hfil \rm\thepage}
    \def\chaptermark##1{\markboth {\uppercase{\ifnum \c@secnumdepth >\m@ne
	\@chapapp\ \thechapter. \ \fi ##1}}{}}
    \def\sectionmark##1{\markright {\uppercase{\ifnum \c@secnumdepth >\z@
	\thesection. \ \fi ##1}}}
    \pulldownheader				% Bring header down from edge
    \pullupfooter				% Bring footer up
}
\else               % If one-sided printing.
\def\ps@drafthead{
    \let\@mkboth\markboth
    \def\@oddfoot{\rm \today \hfil \sc **DRAFT**}
    \def\@oddhead{\hbox{}\sl \rightmark \hfil \rm\thepage}
    \def\chaptermark##1{\markright {\uppercase{\ifnum \c@secnumdepth >\m@ne
	\@chapapp\ \thechapter. \ \fi ##1}}}
    \pulldownheader				% Bring header down from edge
    \pullupfooter				% Bring footer up
}
\fi

% I redefine these formats that were defined in report.sty
% Definition of 'headings' page style 
%  Note the use of ##1 for parameter of \def\chaptermark inside the
%  \def\ps@headings.
%

\if@twoside					% If two-sided printing.
\def\ps@headings{\let\@mkboth\markboth
    \def\@oddfoot{}
    \def\@evenfoot{}		% No feet.
    \def\@evenhead{\rm \thepage\hfil \sl \leftmark}	% Left heading.
    \def\@oddhead{\hbox{}\sl \rightmark \hfil \rm\thepage}	% Right heading.
    \def\chaptermark##1{\markboth {\uppercase{\ifnum \c@secnumdepth >\m@ne
	\@chapapp\ \thechapter. \ \fi ##1}}{}}	
    \def\sectionmark##1{\markright {\uppercase{\ifnum \c@secnumdepth >\z@
	\thesection. \ \fi ##1}}}
    \pulldownheader				% Bring header down from edge
}
\else						% If one-sided printing.
\def\ps@headings{\let\@mkboth\markboth
    \def\@oddfoot{}
    \def\@evenfoot{}		%     No feet.
    \def\@oddhead{\hbox {}\sl \rightmark \hfil \rm\thepage}	% Heading.
    \def\chaptermark##1{\markright {\uppercase{\ifnum \c@secnumdepth >\m@ne
	\@chapapp\ \thechapter. \ \fi ##1}}}
    \pulldownheader				% Bring header down from edge
}
\fi

% Redefinition of 'myheadings' page style.  
%
\def\ps@myheadings{\let\@mkboth\@gobbletwo
    \def\@oddfoot{}
    \def\@evenfoot{}
    \def\sectionmark##1{}
    \def\subsectionmark##1{}
    \def\@evenhead{\rm \thepage\hfil\sl\leftmark\hbox {}}	%
    \def\@oddhead{\hbox{}\sl\rightmark \hfil \rm\thepage}	%
    \pulldownheader				% Bring header down from edge
}						

% Redefine '/chapter' to always start on an odd page.
% Should make no difference in singleside mode.
%
\def\chapter{\cleardoublepage	% Starts new page.
   \thispagestyle{plain}	% Page style of chapter page is 'plain'
   \global\@topnum\z@		% Prevents figures from going at top of page.
   \@afterindentfalse		% Suppresses indent in first paragraph.  Change
   \secdef\@chapter\@schapter}	% to \@afterindenttrue to have indent.

% If using the report style, use - instead of . in the figure number.
\@ifundefined{thechapter}{}{\def\thefigure{\thechapter-\arabic{figure}}}

%%%%%%%%%  End of Style parameters %%%%



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% Define all the pieces that go on the title page and the abstract.
% \title and \author already exist

\def\prevdegrees#1{\gdef\@prevdegrees{#1}}
\def\@prevdegrees{}

\def\department#1{\gdef\@department{#1}}

\def\institute#1{\gdef\@institute{#1}}  %% added by fanghui


% If you are getting two degrees, use \and between the names.
\def\degree#1{\setbox0\hbox{#1}	 %for side effect of setting \@degreeword
  \gdef\@degree{#1}}

% \and is used inside the \degree argument to separate two degrees
\def\and{\gdef\@degreeword{degrees} \par and \par}
\def\@degreeword{degree}

% The copyright notice
%
% \@copyrightnotice is used by \maketitle to actually put text on the
% page; it defaults to ``Copyright SMA 19xx.  All rights reserved.''
% \copyrightnoticetext takes an argument and defined \@copyrightnotice
% to that argument.  \copyrightnotice takes an argument, and calls
% \copyrightnoticetext with that argument, preceeded by a copyright
% symbol and followed by ``All rights reserved.'' and the standard
% permission notice.
% 
% If you use the 'vi' option, \copyrightnoticetext is used to set the
% copyright to ``(C) Your Name, Current Year in Roman Numerals.''
% followed by the permission notice.

% If there is no \copyrightnotice command, it is asssumed that MIT
% holds the copyright.  This commands adds the copyright symbol to the
% beginning, and puts the standard permission notice below.
%% ``All rights reserved'' added.  Krishna Sethuraman (1990)
\def\copyrightnotice#1{\copyrightnoticetext{\copyright\ #1.  All rights
reserved.\par\permission}}

% Occacionally you will need to exactly specify the text of the 
% copyright notice.  The \copyrightnoticetext command is then useful.
\long\def\copyrightnoticetext#1{\gdef\@copyrightnotice{#1}}
% \def\@copyrightnotice{\copyright\ \@institute \ \@degreeyear.  All rights reserved.}  %% fanghui
\def\@copyrightnotice{\copyright\ \@degreeyear \ Nanyang Technological University. All rights reserved.}  %% fanghui


%% `vi' documentclass option: Specifying this option automatically
%% copyrights the thesis to the author and gives MIT permission to copy and
%% distribute the document.  If you want, you can still specify
%% \copyrightnotice{stuff} to copyright to someone else, or
%% \copyrightnoticetext{stuff} to specify the exact text of the copyright
%% notice.
\ifodd\vithesis \copyrightnoticetext{\copyright\ \@author,
\uppercase\expandafter{\romannumeral\@degreeyear}.  All rights reserved.\par\permission}
%% or just
%%\@degreeyear}}
\typeout{Copyright given to author,
	permission to copy/distribute given to SMA.}
\else \typeout{Thesis document copyright SMA unless otherwise (manually) specified}
\fi

\def\thesisdate#1{\gdef\@thesisdate{#1}}

% typically just a month and year
\def\degreemonth#1{\gdef\@degreemonth{#1}}
\def\degreeyear#1{\gdef\@degreeyear{#1}}

% Usage: \supervisor{name}{title}
%        \chairman{name}{title}

% since there can be more than one supervisor,
% we build the appropriate boxes for the titlepage and
% the abstractpage as the user makes multiple calls
% to \supervisor
\newbox\@titlesupervisor 	
\newbox\@abstractsupervisor

\newbox\@titlechairman 	%%% added by fanghui for multi chairmen

\def\supervisor#1#2{
  \setbox\@titlesupervisor\vbox
  {\unvbox\@titlesupervisor \vskip 10pt% plus 1fil minus 1fil
   \def\baselinestretch{1}\large

   \signature{Certified by}{#1 \\ #2 \\ Thesis Advisor}
  }
  
  \setbox\@abstractsupervisor\vbox
  {\unvbox\@abstractsupervisor
   \vskip\baselineskip \def\baselinestretch{1}\@normalsize 
   %\par\noindent Thesis Advisor: #1 \\ Title: #2}}
   #1, #2   %%% replace above line, by fanghui
  }
 }   


%%%% department chairman, not thesis committee chairman
%\def\chairman#1#2{\gdef\@chairmanname{#1}\gdef\@chairmantitle{#2}} %% replaced by below, by fanghui for multi chairmen
\def\chairman#1#2{
  \setbox\@titlechairman\vbox
  {\unvbox\@titlechairman \vskip 10pt% plus 1fil minus 1fil
   \def\baselinestretch{1}\large

   \signature{Accepted by}{#1 \\ #2 }
  }
}




%% `upcase' documentclass option: \choosecase is defined either as a dummy or
%% a macro to change the (expanded) argument to uppercase.
\def\maketitle{\begin{titlepage}
\large
{\def\baselinestretch{1.2}\Large\bf \choosecase{\@title} \par}
by\par
{\Large  \choosecase{\@author}}
\par
\@prevdegrees
\par

%\choosecase{Submitted to the} \choosecase{\@department} \\     %% fanghui
\uppercase{Submitted to the SMA Office in Partial} \\ 
\uppercase{FULFILLMENT OF THE REQUIREMENTS FOR THE DEGREE OF}
%\choosecase{\@degreeword}   %% fanghui
%\choosecase{of}             %% fanghui
\par
%\choosecase{\@degree} \\
\uppercase{Doctor of Philosophy} \\
\choosecase{IN COMPUTER SCIENCE (CS)}  \\
\choosecase{AT THE}
\par

\par\@institute\par    

\@degreemonth\ \@degreeyear
%\par  \@copyrightnotice   \par  %%%% moved to below by fanghui

%%\vskip 3\baselineskip
\vskip 1\baselineskip  %% adjusted by fanghui

%\signature{Signature of author}{\@department \\ \@thesisdate}  %% revised by fanghui
\signature{Signature of author}{CS Programme \\ \@thesisdate}

\par
\vfill
\unvbox\@titlesupervisor
\par
\vfill

%\signature{Accepted by}{\@chairmanname \\ \@chairmantitle}  %% replaced by below, by fanghui for multi chairmen
\par
\vfill
\unvbox\@titlechairman
\par
\vfill
%%%%  above fanghui


\par  \@copyrightnotice   \par    %%% moved from above, by fanghui

\vfill
\end{titlepage}}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% abstract environment 
\def\abstractpage{\newpage
 \begin{center}{\large{\bf \@title} \\
 by \\
 \@author \\[\baselineskip]}
 \par
 \def\baselinestretch{1}\@normalsize
 Submitted to the \@department \\
 In Partial Fulfillment of the Requirements for the \\
 Degree of \@degree\ in Computer Science (CS)
 \end{center}
 \par

 \begin{abstract}   
 }

%% Changed from \unvbox to \unvcopy for use with multiple copies of abstract page.
\def\endabstractpage{
                     \end{abstract}\noindent 
                     \\ \textbf{Keywords:} personal positioning, Bayesian filters, user activity map\\%%% added by fanghui
                     \\ \textbf{Thesis Advisors}:  %%% added this line by fanghui
                     \unvcopy\@abstractsupervisor \newpage
                    }

%% This counter is used to save the page number for the second copy of the abstract.
\newcounter{savepage}

% You can use the titlepage environment to do it all yourself if you
% don't want to use \maketitle.  If the titlepage environment, the
% paragraph skip is infinitely stretchable, so if you leave a blank line
% between lines that you want space between, the space will stretch so
% that the title page fills up the entire page.
\def\titlepage{\newpage\centering
  \thispagestyle{empty}
  \parindent 0pt \parskip 10pt plus 1fil minus 1fil
  \def\baselinestretch{1}\@normalsize\vbox to \vsize\bgroup\vbox to 9in\bgroup}
% The \kern0pt pushes any depth into the height.  Thanks to Richard Stone.
\def\endtitlepage{\par\kern 0pt\egroup\vss\egroup\newpage}



\def\permission{\par\noindent{\centering
   The author hereby grants to SMA permission to reproduce and
   distribute publicly paper and electronic copies of this thesis
   document in whole or in part.}\par}

\def\signature#1#2{\par\noindent#1\dotfill\null\\*
  {\raggedleft #2\par}}

\def\abstract{\subsection*{Abstract}\small\def\baselinestretch{1}\@normalsize}
\def\endabstract{\par}

