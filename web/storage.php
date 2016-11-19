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
				  	<li class="active">
					    <a href="/storage.php">Días anteriores</a>
					</li>
					<li class="">
					    <a href="/graph.php">Gráficas</a>
					</li>
				</ul>
			
			<table class="table table-striped table-hover">
				<thead>
					<th>Fecha</th>
					<th>Acción</th>
				</thead>
			
				<tbody>
					<?php
						$files = scandir("storage/");
					
						
						foreach($files as $file) {
							if($file==".." || $file==".") {
								continue;
							}
							
							if (filesize("storage/".$file)==0) {
                                continue;
                            }
						
							$filename=explode(".",$file);
							
							
							?>
							
							<tr>
								<td><?=$filename[1]?></td>
								<td><a class="btn btn-default" href="/log.php?day=<?=$filename[1]?>" role="button">Ver datos</a></td>
							</tr>
							
							<?php
						}
						
					
					?>
				</tbody>
			</table>
			
			
		</div>
		
		
		
		<footer class="footer">
		      <div class="container">
		        <p class="text-muted">Done with Garagarduino 2.0</p>
		      </div>
		    </footer>
	</body>
</html>