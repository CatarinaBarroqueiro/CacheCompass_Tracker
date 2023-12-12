import { Column, Entity, JoinColumn, JoinTable, ManyToMany, ManyToOne, OneToOne, PrimaryColumn,PrimaryGeneratedColumn } from 'typeorm';
import { User } from '../user/user.entity';
import { Box } from '../box/box.entity';

@Entity({ name: 'Discovery' })
export class Discovery {
    @PrimaryGeneratedColumn()    
    public idDiscovery: number;

    @ManyToOne(() => Box, { onDelete: 'CASCADE' })
    @JoinColumn({ name: 'idBox' })
    public box: Number;
  
    @ManyToOne(() => User, { onDelete: 'CASCADE' })
    @JoinColumn({ name: 'idUser' })
    public user: Number;

    @Column({ default: () => 'CURRENT_TIMESTAMP' })
    discTime: string;

    @Column()
    public authorized: boolean;
}