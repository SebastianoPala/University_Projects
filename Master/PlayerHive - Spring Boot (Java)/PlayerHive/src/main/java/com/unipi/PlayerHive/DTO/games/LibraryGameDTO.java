package com.unipi.PlayerHive.DTO.games;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.NoArgsConstructor;
import lombok.Setter;
import org.springframework.data.annotation.Id;

@AllArgsConstructor
@NoArgsConstructor
@Getter
@Setter

public class LibraryGameDTO {

    @Id
    private String id;
    private String name;
    private Integer achievements;
    private String image;

    // played by
    private Integer achievementsObtained;
    private Double hoursPlayed;
}
