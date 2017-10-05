/* 
  ESERCIZI 
  - studia CSS per aprire e chiudere roba, cioe' per mettere e togliere i <div>

*/


document.write("Hello World!")
function test ()
{
  document.write ("preso!\n") ;
}


function test1 ()
{
  alert ("successo :)") ;
}


window = new Array ()
windowByName = new Array ()

function getWindow (n) { return windowByName[n] ; }

window.prototype.initialize = defineWindow ;

function detach (name)
{
  alert (name) ;
  coso = document.getElementById (name) ;
//  document.wirte ("preso!\n") ;
}


// template object instantiation
function Window (name)  {
    this.index=window.length ;
    window.push (this) ;
    windowByName[name] = this ;
    this.name=name ;
/*
    this.expand=expand ; 
    this.collapse=collapse ;
    this.changeStatus= (isOpen==0) ? expand : collapse ;
    this.drawBar = drawBar
    this.drawText = drawText
    this.isOpen=0 ;
    this.buttons=new Array() ;
*/
  }

function defineWindow ()
{
	this.win  = document.getElementById (this.name) ;
}

/*
// use the template to do it
// window[1] = new Window ("second") 


function changeStatus ()
  {
    if (isOpen==0) this.expand()
    else this.collapse()
  }


function collapse ()
  {
    isOpen = 0 
  }


function expand ()
  {
    isOpen = 1 
  }
  
function drawBar ()
  {}

function drawText ()
  {}

*/