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
					<li class="active">
				    	<a href="/log.php">Hoy</a>
				  	</li>
				  	<li class="">
					    <a href="/storage.php">Días anteriores</a>
					</li>
					<li class="">
					    <a href="/graph.php">Gráficas</a>
					</li>
				</ul>
			
			<table class="table table-striped table-hover">
				<thead>
					<th>Fecha</th>
					<th>Temperatura externa</th>
					<th>Temperatura del mosto</th>
					<th>Calor</th>
					<th>Frío</th>
				</thead>
			
				<tbody>
					<?php
						if(isset($_GET["day"])) {
							$day=$_GET["day"];
							$file="storage/data.$day.csv";
						} else {
							$file="data.csv";
						}
					
						$data=file_get_contents($file);
						
						$garagarduino_data=explode("\n",$data);
						
						foreach(array_reverse($garagarduino_data) as $line) {
							$sensors_data=explode(",",$line);
							
							if($sensors_data[0]!="") {
								if($sensors_data[3]==0) {
									$hot="<i class=\"fa fa-toggle-off\"></i>";
								} else {
									$hot="<font color=\"red\"><i class=\"fa fa-toggle-on\"></i></font>";
								}
								
								if($sensors_data[4]==0) {
									$cold="<i class=\"fa fa-toggle-off\"></i>";
								} else {
									$cold="<font color=\"blue\"><i class=\"fa fa-toggle-on\"></i></font>";
								}
							
							?>
							<tr>
								<td><?=$sensors_data[0]?></td>
								<td align="center"><?=$sensors_data[1]?> ºC</td>
								<td align="center"><?=$sensors_data[2]?> ºC</td>
								<td><?=$hot?></td>
								<td><?=$cold?></td>
							
							</tr>
							
							<?
							}
						}
						
					
					?>
				</tbody>
			</table>
			
			
		</div>
		
		
		
		 <?php
            require_once("footer.php");
        ?>
	</body>
</html>