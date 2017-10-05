$folder = $ARGV[0] ;
$file = folder.".tex" ;

foreach file (`ls $folder`)
{
  
}


sub openLatex ()
{
open (LATEX,">$file") ;
print LATEX <<EOF ; 
\documentclass[11pt]{article}
\usepackage{geometry}                % See geometry.pdf to learn the layout options. There are lots.
\geometry{letterpaper}                   % ... or a4paper or a5paper or ... 
%\geometry{landscape}                % Activate for for rotated page geometry
%\usepackage[parfill]{parskip}    % Activate to begin paragraphs with an empty line rather than an indent
\usepackage{graphicx}
\usepackage{amssymb}
\usepackage{epstopdf}
\DeclareGraphicsRule{.tif}{png}{.png}{`convert #1 `dirname #1`/`basename #1 .tif`.png}
\begin{document}

EOF
close (LATEX) ;
}

sub putPic ()
{
$fig_file = $_[0] ;
open (LATEX,">>$file") ;
print LATEX <<EOF ; 
\begin{figure}[htbp]
   \centering
   \includegraphics{wenu.jpg} 
   \caption{$fig_file} 
\end{figure}}
EOF
close (LATEX) ;

sub closeLatex ()
{
open (LATEX,">>$file") ;
print LATEX <<EOF ; 
\end{document}  
EOF
close (LATEX) ;
}