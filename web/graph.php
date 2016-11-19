<?php

if(isset($_GET["day"])) {
    $data_csv="storage/data.".$_GET["day"].".csv";
} else {
    $data_csv="data.csv";
}



$batch="dev";

?>;

<html>
	<head>
		<?php
	        require_once("header.php");
	    ?>
	</head>
	
	<body>
		
		<div class="container">
			
			<div class="page-header">
				<h2>Garagarduino 3.0 Datalog <i class="fa fa-beer"></i></h2>
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
				
				
				<?php
				if(isset($_GET["type"])) {
                    if($_GET["type"]=="graphite") {
                    ?>
                    <p>
                        <p class="lead">Temperatura externa</p>
    				    <img src="http://statsd.server.tld/render/?width=750&height=308&_salt=1464163889.233&target=stats.gauges.Homebrew.batch_<?=$batch?>.air_temperature&lineWidth=2&bgcolor=white&fgcolor=black&colorList=green&hideLegend=true" />
    				</p>
    				
    				<p>
    				    <p class="lead">Temperatura del mosto</p>
    				    <img src="http://statsd.server.tld/render/?width=750&height=308&_salt=1464163889.233&target=stats.gauges.Homebrew.batch_<?=$batch?>.wort_temperature&lineWidth=2&bgcolor=white&fgcolor=black&colorList=brown&hideLegend=true" />
    				</p>
    				
    				<p>
    				    <p class="lead">Calor</p>
    				    <img src="http://statsd.server.tld/render/?width=750&height=308&_salt=1464163889.233&target=stats.gauges.Homebrew.batch_<?=$batch?>.hot&lineWidth=2&bgcolor=white&fgcolor=black&colorList=red&hideLegend=true" />
    				</p>
    				
    				<p>
    				    <p class="lead">Frío</p>
    				    <img src="http://statsd.server.tld/render/?width=750&height=308&_salt=1464163889.233&target=stats.gauges.Homebrew.batch_<?=$batch?>.cold&lineWidth=2&bgcolor=white&fgcolor=black&colorList=blue&hideLegend=true" />
    				</p>

                    
                    <?php
                    } 
                } else {
                
                ?>
                    <p>
                        <p class="lead">Datos de temperatura</p>
                        <div id="full" style="width: 850px; height: 250px;"><!-- Plotly chart will be drawn inside this DIV --></div>
                    </p>
    				
    				
    				
    				
    				<script>
                    Plotly.d3.csv('<?php echo $data_csv;?>', function(rows){
                    var trace_air = {
                      type: 'scatter',                    // set the chart type
                      mode: 'lines',                      // connect points with lines
                      name: 'Aire',
                      x: rows.map(function(row){          // set the x-data
                        return row['date'].split(" ")[1];
                      }),
                      y: rows.map(function(row){          // set the x-data
                        return row['air_temp'];
                      }),
                      line: {                             // set the width of the line.
                        color: 'darkturquoise',
                        width: 1
                      }
                    };
                    
                    var trace_wort = {
                      type: 'scatter',                    // set the chart type
                      mode: 'lines',                      // connect points with lines
                      name: 'Mosto',
                      x: rows.map(function(row){          // set the x-data
                        return row['date'].split(" ")[1];
                      }),
                      y: rows.map(function(row){          // set the x-data
                        return row['wort_temp'];
                      }),
                      line: {                             // set the width of the line.
                        color: 'brown',
                        width: 1
                      }
                    };
                    
                    var trace_hot = {
                      type: 'scatter',                    // set the chart type
                      mode: 'lines',                      // connect points with lines
                      name: 'Calor',
                      x: rows.map(function(row){          // set the x-data
                        return row['date'].split(" ")[1];
                      }),
                      y: rows.map(function(row){          // set the x-data
                        return row['hot'];
                      }),
                      line: {                             // set the width of the line.
                        color: 'red',
                        width: 1
                      }
                    };
                    
                    var trace_cold = {
                      type: 'scatter',                    // set the chart type
                      mode: 'lines',                      // connect points with lines
                      name: 'Frío',
                      x: rows.map(function(row){          // set the x-data
                        return row['date'].split(" ")[1];
                      }),
                      y: rows.map(function(row){          // set the x-data
                        return row['cold'];
                      }),
                      line: {                             // set the width of the line.
                        color: 'blue',
                        width: 1
                      }
                    };
                
                    var layout = {
                      yaxis: {title: ""},       // set the y axis title
                      xaxis: {
                        showgrid: false,                  // remove the x-axis grid lines
                        //tickformat: "%B, %Y"              // customize the date format to "month, day"
                      },
                      margin: {                           // update the left, bottom, right, top margin
                        l: 40, b: 45, r: 10, t: 20
                      }
                    };
                
                    var full_data = [trace_air,trace_wort,trace_hot,trace_cold];
                    
                    Plotly.plot(document.getElementById('full'), full_data, layout, {showLink: false});

                });
                    </script>
                
                
                <?php
                }
				?>
				
								
			</div>

		</div>
	
		<?php
		  require_once("footer.php");
		?>
	</body>
</html>