import { Component, OnInit } from '@angular/core';
import { MatTableModule } from '@angular/material/table';
import { HttpClient, HttpClientModule } from '@angular/common/http';

export interface GeocacheDiscovery {
  user: string;
  timestamp: string;
  cacheId: number;
  location: string;
}

@Component({
  selector: 'app-history',
  standalone: true,
  imports: [MatTableModule, HttpClientModule],
  templateUrl: './history.component.html',
  styleUrls: ['./history.component.css']
})
export class HistoryComponent implements OnInit {
  displayedColumns: string[] = ['user', 'timestamp', 'cacheId', 'location'];
  dataSource: GeocacheDiscovery[] = [];

  constructor(private http: HttpClient) {}

  ngOnInit() {
    // Fetch data from the API when the component initializes
    this.fetchDiscoveryData();
  }

  fetchDiscoveryData() {
    const apiUrl = 'http://localhost:3000/discovery';

    this.http.get<{ data: GeocacheDiscovery[] }>(apiUrl).subscribe(
      (data) => {
        console.log('Fetched data:', data.data);
        
        // Modify the timestamp property to truncate it to 6 characters
        this.dataSource = data.data.map(item => ({
          ...item,
          timestamp: item.timestamp ? item.timestamp.substring(0, 6) : ''
        }));
      },
      (error) => {
        console.error('Error fetching data:', error);
      }
    );
  }
}
