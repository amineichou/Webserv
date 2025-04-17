<?php

// Get all environment variables
$envVars = getenv();

// Set the header for HTML content
echo "Content-type: text/html\n";
echo "\n";

// Start the HTML document
echo "<html><head><title>Environment Variables</title>";
echo "<style>";
echo "body { font-family: Arial, sans-serif; background-color: #f4f4f9; margin: 0; padding: 0; }";
echo "h1 { text-align: center; color: #333; padding: 20px; background-color: #4CAF50; color: white; margin: 0; }";
echo "table { width: 100%; border-collapse: collapse; margin: 20px; }";
echo "th, td { padding: 10px; text-align: left; border: 1px solid #ddd; }";
echo "th { background-color: #f2f2f2; color: #333; }";
echo "tr:nth-child(even) { background-color: #f9f9f9; }";
echo "</style></head><body>";

// Title of the page
echo "<h1>Environment Variables</h1>";

// Start the table
echo "<table>";

// Table headers
echo "<tr><th>Variable</th><th>Value</th></tr>";

// Loop through each environment variable and print it
foreach ($envVars as $key => $value) {
    echo "<tr><td>" . htmlspecialchars($key) . "</td><td>" . htmlspecialchars($value) . "</td></tr>";
}

// Close the table and body
echo "</table></body></html>";
?>
