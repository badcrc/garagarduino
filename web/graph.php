<html>
	<head>
	    <?php
	        require_once("header.php");
	    ?>
	</head>
	
	<body>
		
		<div class="container">
			
			<div class="page-header">
				<h2>Garagarduino 2.0 Datalog <i class="fa fa-beer"></i></h2>
			</div>
			
			<ul class="nav nav-tabs">
					<li class="">
				    	<a href="/log.php">Hoy</a>
				  	</li>
				  	<li class="">
					    <a href="/storage.php">Días anteriores</a>
					</li>
					<li class="active">
					    <a href="/graph.php">Gráficas</a>
					</li>
					
				</ul>
			<div align="">
				<p>
				<p class="lead">Temperatura externa</p>
				<?php
					$batch="dev";
				?>
				<img src="http://graphite.server.tld/render/?width=750&height=308&_salt=1464163889.233&target=stats.gauges.Homebrew.batch_<?=$batch?>.air_temperature&lineWidth=2&bgcolor=white&fgcolor=black&colorList=green&hideLegend=true" />
				</p>
				
				<p>
				<p class="lead">Temperatura del mosto</p>
				<?php
					$batch="2";
				?>
				<img src="http://graphite.server.tld/render/?width=750&height=308&_salt=1464163889.233&target=stats.gauges.Homebrew.batch_<?=$batch?>.wort_temperature&lineWidth=2&bgcolor=white&fgcolor=black&colorList=brown&hideLegend=true" />
				</p>
				
				<p>
				<p class="lead">Calor</p>
				<?php
					$batch="2";
				?>
				<img src="http://graphite.server.tld/render/?width=750&height=308&_salt=1464163889.233&target=stats.gauges.Homebrew.batch_<?=$batch?>.hot&lineWidth=2&bgcolor=white&fgcolor=black&colorList=red&hideLegend=true" />
				</p>
				
				<p>
				<p class="lead">Frío</p>
				<?php
					$batch="2";
				?>
				<img src="http://graphite.server.tld/render/?width=750&height=308&_salt=1464163889.233&target=stats.gauges.Homebrew.batch_<?=$batch?>.cold&lineWidth=2&bgcolor=white&fgcolor=black&colorList=blue&hideLegend=true" />
				</p>
				
			</div>
			
			
			
		</div>
		
		
		
		<footer class="footer">
		      <div class="container">
		        <p class="text-muted">Done with Garagarduino 2.0</p>
		      </div>
		    </footer>
	</body>
</html>