import { Component, OnInit, ViewChild } from '@angular/core';
import { MatTableModule } from '@angular/material/table';
import { HttpClient, HttpClientModule } from '@angular/common/http';
import { MatPaginator, MatPaginatorModule } from '@angular/material/paginator';

export interface GeocacheDiscovery {
  user: string;
  discTime: string;
  idBox: number;
  local: string;
}

export interface Box {
  idBox: number;
  local: string;
}

@Component({
  selector: 'app-history',
  standalone: true,
  imports: [MatTableModule, HttpClientModule, MatPaginatorModule],
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
    const apiUrl = 'http://51.20.64.70:3000/discovery';
    const boxUrl = 'http://51.20.64.70:3000/box/idBox?idBox=';

    this.http.get<{ data: GeocacheDiscovery[] }>(apiUrl).subscribe(
      (data) => {
        console.log('Fetched data from discovery API:', data.data);

        // Modify the timestamp property to a string
        const modifiedData = data.data.map(item => ({
          ...item,
          local : "Universidade de Aveiro"
        }));

        // Set the data property of MatTableDataSource
        this.dataSource = modifiedData;

        // Fetch data from the box API using idBox from the first API

        console.log('DATA SOURCE:');
        this.dataSource.forEach(item => {
          console.log(item.idBox);
        });

        this.dataSource.forEach(item => {
          console.log(item.idBox);

          // if (item.idBox !== undefined) {
          //   this.http.get<{ data: Box[] }>(boxUrl + item.idBox).subscribe(              
          //     (boxData) => {

          //       if (boxData?.data && boxData.data.length > 0) {
          //         console.log('Fetched data from box API:', boxData?.data[0]);
          //         // Update the dataSource.data with the location data from the box API
          //         this.dataSource = this.dataSource.map(d => 
          //           d.idBox === item.idBox ? { ...d, local: boxData.data[0]?.local } : d
          //         );
          //       } else {
          //         console.warn('Box data is undefined or empty for idBox:', item.idBox);
          //       }
          
          //       // Update the dataSource.data with the location data from the box API
          //       this.dataSource = this.dataSource.map(d => 
          //         d.idBox === item.idBox ? { ...d, local: boxData.data[0]?.local } : d
          //       );
          //     },
          //     (error) => {
          //       console.error('Error fetching data from box API:', error);
          //     }
          //   );
          // }
        }
    );});

  }
  
  

}
